#include <memory>
#include <recollect.hpp>
#include <tao/pq/connection.hpp>

#include "threadlocal.hpp"

namespace ThreadLocal {
// PostgreSQL connection
thread_local std::shared_ptr<tao::pq::connection> conn;
// memcached connection
thread_local std::unique_ptr<recollect::Memcache> cache;
// thread ID
thread_local unsigned int tid;
}  // namespace ThreadLocal
