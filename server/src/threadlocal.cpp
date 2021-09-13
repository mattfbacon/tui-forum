#include <libmemcached/memcached.hpp>
#include <memory>
#include <tao/pq/connection.hpp>

#include "threadlocal.hpp"

namespace ThreadLocal {
// PostgreSQL connection
thread_local std::shared_ptr<tao::pq::connection> conn;
// memcached connection
thread_local std::unique_ptr<memcache::Memcache> cache;
// thread ID
thread_local unsigned int tid;
}  // namespace ThreadLocal
