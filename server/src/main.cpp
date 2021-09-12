#include <iostream>
#include <libmemcached/memcached.hpp>
#include <mariadb/conncpp.hpp>
#include <uWebSockets/App.h>

#include "Config.hpp"
#include "Routes.hpp"

#include "threadlocal.hpp"

struct ConnectionError : public std::exception {
	enum class Service {
		mariadb,
		memcached,
	};
	Service service;
	ConnectionError(Service const service) : service(service) {}
	char const* what() const noexcept override {
		return "ConnectionError";
	}
	char const* service_name() const noexcept {
		switch (service) {
			case Service::mariadb:
				return "MariaDB";
			case Service::memcached:
				return "Memcached";
			default:
				__builtin_unreachable();
		}
	}
};

auto connect_to_db() {
	// because accessing a constant SQLString is apparently not thread-safe...
	static std::mutex db_config_mutex;
	std::lock_guard _lock{ db_config_mutex };
	namespace C = MariaDBConfig;
	try {
		return std::unique_ptr<sql::Connection>{ sql::DriverManager::getConnection("jdbc:mariadb://" + C::host + "/" + C::database, C::username, C::password) };
	} catch (sql::SQLSyntaxErrorException const& e) {
		// 11 = can't connect
		if (const_cast<sql::SQLSyntaxErrorException&>(e).getErrorCode() == 11) {
			throw ConnectionError{ ConnectionError::Service::mariadb };
		} else {
			throw;
		}
	}
}

auto connect_to_memcached() {
	namespace C = MemcachedConfig;
	auto conn = std::make_unique<memcache::Memcache>(C::sock_path);
	if (conn->error()) {
		throw ConnectionError{ ConnectionError::Service::memcached };
	}
	return conn;
}

class ThreadInfo {
	std::mutex info_mutex;
	unsigned int goal_num;
	unsigned int num_online;
public:
	ThreadInfo() {
		goal_num = std::thread::hardware_concurrency();
	}
	auto get_goal_num() {
		return goal_num;
	}
	void thread_online() {
		std::lock_guard _lock{ info_mutex };
		if (isatty(fileno(stderr))) {
			std::clog << "\x1b[" << std::to_string(strlen("Status: ") + 1 + ThreadLocal::tid) << "G\x1b[32m✓\x1b[0m";
		} else {
			std::clog << "thread #" << ThreadLocal::tid << " online\n";
		}
		num_online++;
		if (num_online >= goal_num) {
			if (isatty(fileno(stderr))) {
				std::clog << '\n';
			}
			std::clog << "All threads are online\nListening on port " << WebConfig::PORT << '\n';
		}
		std::clog << std::flush;
	}
};  // namespace thread_info
ThreadInfo thread_info;

void listen_callback(us_listen_socket_t* const) {
	thread_info.thread_online();
}

void create_server(unsigned int const thread_id) {
	ThreadLocal::tid = thread_id;
	try {
		if (ThreadLocal::conn.get() == nullptr) {
			ThreadLocal::conn = connect_to_db();
		}
		if (ThreadLocal::cache.get() == nullptr) {
			ThreadLocal::cache = connect_to_memcached();
		}
	} catch (ConnectionError const& e) {
		if (isatty(fileno(stderr))) {
			std::clog << "\x1b[0G";
		}
		std::clog << "Could not connect to " << e.service_name() << "is the service running? " << std::endl;
		_exit(EXIT_FAILURE);
	}
	Routes::register_all(uWS::App{}).listen(WebConfig::PORT, listen_callback).run();
}

int main() {
	std::clog << "Using " << thread_info.get_goal_num() << " threads." << std::endl;
	if (isatty(fileno(stderr))) {
		std::clog << "Status: \x1b[34m";
		for (size_t i = 0; i < thread_info.get_goal_num(); i++) {
			std::clog << "o";
		}
		std::clog << "\x1b[0m" << std::flush;
	}
	std::vector<std::unique_ptr<std::thread>> threads{ thread_info.get_goal_num() };
	for (unsigned int current_thread_id = 0; current_thread_id < thread_info.get_goal_num(); current_thread_id++) {
		threads[current_thread_id] = std::make_unique<std::thread>(create_server, current_thread_id);
	}
	for (auto& thread : threads) {
		thread->join();
	}
}
