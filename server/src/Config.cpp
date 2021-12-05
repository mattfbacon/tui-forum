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

}  // namespace Config
