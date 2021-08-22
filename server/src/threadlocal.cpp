#include <mariadb/conncpp/Connection.hpp>
#include <memory>

#include "threadlocal.hpp"

namespace ThreadLocal {
// MariaDB connection
thread_local std::unique_ptr<sql::Connection> conn;
// thread ID
thread_local unsigned int tid;
}  // namespace ThreadLocal
