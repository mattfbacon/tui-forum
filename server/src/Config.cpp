#include "Config.hpp"

namespace MariaDBConfig {
sql::SQLString const host = "localhost";
sql::SQLString const database = "tuiforum";
sql::SQLString const username = "test";
sql::SQLString const password = "test";
}  // namespace MariaDBConfig

namespace MemcachedConfig {
std::string const sock_path = "/run/memcached/memcache.sock";
}
