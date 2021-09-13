#include <msgpack/unpack.hpp>
#include <optional>

#include "util.hpp"

namespace util {

void write_sv_to_unpacker(msgpack::unpacker& unpacker, std::string_view const sv) {
	unpacker.reserve_buffer(sv.size());
	std::copy(std::cbegin(sv), std::cend(sv), unpacker.buffer());
	unpacker.buffer_consumed(sv.size());
}

}  // namespace util

namespace Strings {
std::string const SELF = "self";
std::string const SQL_GET_ROW_COUNT = "select row_count()";
}  // namespace Strings
