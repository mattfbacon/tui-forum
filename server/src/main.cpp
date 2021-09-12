#include <iostream>
#include <libmemcached/memcached.hpp>
#include <mariadb/conncpp.hpp>
#include <uWebSockets/App.h>

#include "Config.hpp"
#include "Routes.hpp"

#include "threadlocal.hpp"

auto connect_to_db() {
	// because accessing a constant SQLString is apparently not thread-safe...
	static std::mutex db_config_mutex;
	std::lock_guard _lock{ db_config_mutex };
	namespace C = MariaDBConfig;
	return std::unique_ptr<sql::Connection>{ sql::DriverManager::getConnection("jdbc:mariadb://" + C::host + "/" + C::database, C::username, C::password) };
}

auto connect_to_memcached() {
	namespace C = MemcachedConfig;
	auto conn = std::make_unique<memcache::Memcache>(C::sock_path);
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
	if (ThreadLocal::conn.get() == nullptr) {
		ThreadLocal::conn = connect_to_db();
	}
	if (ThreadLocal::cache.get() == nullptr) {
		ThreadLocal::cache = connect_to_memcached();
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
