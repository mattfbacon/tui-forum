#pragma once

#include <recollect.hpp>
#include <string>
#include <vector>

namespace SqlConfig {
extern std::string const host;
extern std::string const database;
extern std::string const username;
extern std::string const password;
}  // namespace SqlConfig

namespace WebConfig {
static constexpr int const PORT = 9000;
static constexpr bool const USE_SSL = false;
}  // namespace WebConfig

namespace MemcachedConfig {
extern std::vector<recollect::ConnectionMeans> const means;
}
