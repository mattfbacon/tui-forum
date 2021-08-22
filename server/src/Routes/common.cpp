#include "HTTP.hpp"
#include "Routes.hpp"

namespace Routes {
std::string const splat_route = "/*";

void send_code_handler(Response& res, HTTP::Status::code_t const code) {
	res.writeStatus(HTTP_STATUS(code));
	res.write(HTTP_STATUS(code));
	res.end();
}

uWS::App&& register_404_handler(uWS::App&& app) {
	return app.any(splat_route, send_code_handler<HTTP::Status::NOT_FOUND>);
}
uWS::App&& register_all(uWS::App&& app) {
	// TODO when handlers are implemented: add here
	return register_404_handler(users::id::register_all(std::move(app)));
}
}  // namespace Routes
