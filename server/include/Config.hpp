#pragma once

#include <recollect.hpp>
#include <string>
#include <vector>

namespace Config {

namespace Web {
extern int port;
}

namespace Sql {
extern std::string host;
extern int port;
extern std::string database;
extern std::string username;
extern std::string password;
}  // namespace Sql

namespace Memcached {
extern std::vector<recollect::ConnectionMeans> means;
}

namespace Security {
constexpr size_t TOKEN_SIZE = 60;
}

}  // namespace Config
