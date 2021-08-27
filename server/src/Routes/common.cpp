#include "HTTP.hpp"
#include "Routes.hpp"

namespace Routes {
std::string const splat_route = "/*";

void send_code_handler(Response& res, HTTP::Status::code_t const code) {
	res.writeStatus(HTTP_STATUS(code));
	res.write(HTTP_STATUS(code));
	res.end();
}

ROUTES_CUSTOM_REGISTERER_IMPL(404_handler, app) {
	return app.any(splat_route, send_code_handler<HTTP::Status::NOT_FOUND>);
}
ROUTES_REGISTERER_IMPL(app) {
	// clang-format off
	return
		register_404_handler(
		posts::register_all(
		users::register_all(
			std::move(app)
		)
		)
		)
		;
	// clang-format on
}
}  // namespace Routes
