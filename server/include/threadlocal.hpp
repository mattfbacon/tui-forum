#pragma once

namespace sql {
class Connection;
}
namespace memcache {
class Memcache;
}
#include <memory>

namespace ThreadLocal {
extern thread_local std::unique_ptr<sql::Connection> conn;
extern thread_local std::unique_ptr<memcache::Memcache> cache;
extern thread_local unsigned int tid;
}  // namespace ThreadLocal
