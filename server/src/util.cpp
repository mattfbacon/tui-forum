#include <msgpack/unpack.hpp>
#include <optional>

#include "HTTP.hpp"

#include "util.hpp"

namespace HTTP {

namespace Status {
#define STATUS_CODE(NAME, TEXT) \
	std::pair<code_t, std::string> { \
		NAME, std::to_string(NAME) + " " TEXT \
	}
std::unordered_map<code_t, std::string> const strings{
	STATUS_CODE(OK, "OK"),
	STATUS_CODE(NO_CONTENT, "No Content"),
	STATUS_CODE(BAD_REQUEST, "Bad Request"),
	STATUS_CODE(UNAUTHORIZED, "Unauthorized"),
	STATUS_CODE(FORBIDDEN, "Forbidden"),
	STATUS_CODE(NOT_FOUND, "Not Found"),
	STATUS_CODE(METHOD_NOT_ALLOWED, "Method Not Allowed"),
	STATUS_CODE(I_M_A_TEAPOT, "I'm a Teapot"),
	STATUS_CODE(UNPROCESSABLE_ENTITY, "Unprocessable Entity"),
	STATUS_CODE(INTERNAL_SERVER_ERROR, "Internal Server Error"),
};
}  // namespace Status

char const* StatusException::what() const noexcept {
	return "HTTP::StatusException (this should have been caught!)";
}
}  // namespace HTTP

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
