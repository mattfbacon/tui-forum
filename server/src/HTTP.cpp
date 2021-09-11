#include "HTTP.hpp"

namespace HTTP {

namespace Status {
#define STATUS_CODE(NAME, TEXT) \
	std::pair<code_t, std::string> { \
		NAME, std::to_string(NAME) + " " TEXT \
	}
std::unordered_map<code_t, std::string> const strings{
	STATUS_CODE(OK, "OK"),
	STATUS_CODE(CREATED, "Created"),
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

void send_code_handler(Response& res, Status::code_t const code) {
	res.writeStatus(HTTP_STATUS(code));
	res.write(HTTP_STATUS(code));
	res.end();
}

}  // namespace HTTP
