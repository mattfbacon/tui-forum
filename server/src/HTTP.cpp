#include "HTTP.hpp"

namespace HTTP {

void send_code_handler(Response& res, HTTP::Status::code_t const code) {
	res.writeStatus(HTTP_STATUS(code));
	res.write(HTTP_STATUS(code));
	res.end();
}

}  // namespace HTTP
