#include <charconv>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string_view>

#include "Config.hpp"

namespace Config {

namespace Web {
int port = 9000;
}

namespace Sql {
std::string host = "localhost";
int port = 5432;
std::string database = "tuiforum";
std::string username = "test";
std::string password = "test";
}  // namespace Sql

namespace Memcached {
using recollect::ConnectionMeans;
std::vector<ConnectionMeans> means{
	ConnectionMeans::unix_socket("/run/memcached/memcache.sock"),
};
}  // namespace Memcached

namespace {

template <typename T>
concept is_integral_concept = std::is_integral_v<T>;

template <is_integral_concept T>
std::optional<T> parse_int(std::string_view raw) {
	T ret;
	auto const [ptr, ec] = std::from_chars(raw.begin(), raw.end(), ret);
	if (ptr != raw.end() || ec != std::errc{}) {
		return std::nullopt;
	}
	return ret;
}

struct ParseError : public std::exception {
	std::string key;
	std::string value;
	ParseError(std::string a_key, std::string a_value) : key(std::move(a_key)), value(std::move(a_value)) {}
	char const* what() const noexcept override {
		return "ParseError";
	}
};

template <typename T>
void set_from_env(char const* const name, T& x) {
	if (auto const* env_value = std::getenv(name)) {
		x = env_value;
	}
}

template <typename T>
void set_from_env(char const* const name, T& x) requires is_integral_concept<T> {
	if (auto const* env_value = std::getenv(name)) {
		std::string_view sv{ env_value, std::strlen(env_value) };
		auto ret = parse_int<T>(sv);
		if (!ret) {
			throw ParseError{ std::string{ name }, std::string{ sv } };
		}
		x = *ret;
	}
}

}  // namespace

bool insert_connection_mean(std::vector<recollect::ConnectionMeans>& means, std::string_view raw_mean) {
	if (raw_mean.starts_with('/')) {
		means.emplace_back(Memcached::ConnectionMeans::unix_socket(std::string{ raw_mean }));
	} else {
		auto const colon_pos = raw_mean.find_last_of(':');
		if (colon_pos == std::string_view::npos) {
			means.emplace_back(Memcached::ConnectionMeans::tcp(std::string{ raw_mean }));
		} else {
			std::optional const port = parse_int<int>(raw_mean.substr(colon_pos + 1));
			if (!port) {
				return false;
			}
			means.emplace_back(Memcached::ConnectionMeans::tcp(std::string{ raw_mean.substr(0, colon_pos) }, *port));
		}
	}
	return true;
}

void load_from_env() try {
	set_from_env("TUIFORUM_PORT", Web::port);
	set_from_env("TUIFORUM_SQL_HOST", Sql::host);
	set_from_env("TUIFORUM_SQL_PORT", Sql::port);
	set_from_env("TUIFORUM_SQL_DATABASE", Sql::database);
	set_from_env("TUIFORUM_SQL_USERNAME", Sql::username);
	set_from_env("TUIFORUM_SQL_PASSWORD", Sql::password);
	if (auto const* env_value = std::getenv("TUIFORUM_MEMCACHED")) {
		Config::Memcached::means.clear();
		std::string_view means_sv{ env_value, std::strlen(env_value) };
		// FIXME when std::ranges::split_view works correctly
		while (!means_sv.empty()) {
			auto pos = means_sv.find(';');
			if (pos == std::string_view::npos) {
				pos = means_sv.size();
			}
			auto const mean_sv = means_sv.substr(0, pos);
			if (!insert_connection_mean(Config::Memcached::means, mean_sv)) {
				throw ParseError{ "TUIFORUM_MEMCACHED", std::string{ mean_sv } };
			}
			means_sv.remove_prefix(pos);
		}
	}
} catch (ParseError const& e) {
	std::clog << "Could not parse " << std::quoted(e.key) << " from environment (value: " << std::quoted(e.value) << ')' << std::endl;
}

}  // namespace Config
