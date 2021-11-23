#pragma once

namespace tao {
namespace pq {
class connection;
}  // namespace pq
}  // namespace tao
namespace recollect {
class Memcache;
}
#include <memory>
#include <random>

namespace ThreadLocal {
extern thread_local std::shared_ptr<tao::pq::connection> conn;
extern thread_local std::unique_ptr<recollect::Memcache> cache;
extern thread_local unsigned int tid;
extern thread_local std::random_device random_device;
}  // namespace ThreadLocal
