#include <msgpack/unpack.hpp>
#include <optional>

#include "util.hpp"

namespace Strings {
std::string const SELF = "self";
std::string const SQL_GET_ROW_COUNT = "select row_count()";
}  // namespace Strings

std::string sqlstr_to_str(sql::SQLString const& str) {
	return std::string{ std::cbegin(str), std::cend(str) };
}
sql::SQLString str_to_sqlstr(std::string const& str) {
	return sql::SQLString{ str.data(), str.size() };
}

void write_sv_to_unpacker(msgpack::unpacker& unpacker, std::string_view const sv) {
	unpacker.reserve_buffer(sv.size());
	std::copy(std::cbegin(sv), std::cend(sv), unpacker.buffer());
	unpacker.buffer_consumed(sv.size());
}
