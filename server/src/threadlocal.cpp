#include <libmemcached/memcached.hpp>
#include <mariadb/conncpp/Connection.hpp>
#include <memory>

#include "threadlocal.hpp"

namespace ThreadLocal {
// MariaDB connection
thread_local std::unique_ptr<sql::Connection> conn;
// memcached connection
thread_local std::unique_ptr<memcache::Memcache> cache;
// thread ID
thread_local unsigned int tid;
}  // namespace ThreadLocal
