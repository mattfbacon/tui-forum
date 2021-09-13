#pragma once

namespace tao {
namespace pq {
class connection;
}  // namespace pq
}  // namespace tao
namespace memcache {
class Memcache;
}
#include <memory>

namespace ThreadLocal {
extern thread_local std::shared_ptr<tao::pq::connection> conn;
extern thread_local std::unique_ptr<memcache::Memcache> cache;
extern thread_local unsigned int tid;
}  // namespace ThreadLocal
