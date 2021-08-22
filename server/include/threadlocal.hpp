#pragma once

namespace sql {
class Connection;
}
#include <memory>

namespace ThreadLocal {
extern thread_local std::unique_ptr<sql::Connection> conn;
extern thread_local unsigned int tid;
}  // namespace ThreadLocal
