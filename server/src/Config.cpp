#include "Config.hpp"

namespace SqlConfig {
std::string const host = "localhost";
std::string const database = "tuiforum";
std::string const username = "test";
std::string const password = "test";
}  // namespace SqlConfig

namespace MemcachedConfig {
std::string const sock_path = "/run/memcached/memcache.sock";
}
