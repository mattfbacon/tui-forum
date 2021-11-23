#include "Config.hpp"

namespace SqlConfig {
std::string const host = "localhost";
std::string const database = "tuiforum";
std::string const username = "test";
std::string const password = "test";
}  // namespace SqlConfig

namespace MemcachedConfig {
using recollect::ConnectionMeans;
std::vector<ConnectionMeans> const means{
	ConnectionMeans::unix_socket("/run/memcached/memcache.sock"),
};
}  // namespace MemcachedConfig
