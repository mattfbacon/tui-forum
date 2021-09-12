#pragma once

#include <mariadb/conncpp.hpp>

namespace MariaDBConfig {
extern sql::SQLString const host;
extern sql::SQLString const database;
extern sql::SQLString const username;
extern sql::SQLString const password;
}  // namespace MariaDBConfig

namespace WebConfig {
static constexpr int const PORT = 9000;
static constexpr bool const USE_SSL = false;
}  // namespace WebConfig

namespace MemcachedConfig {
extern std::string const sock_path;
}
