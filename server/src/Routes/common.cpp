#include "HTTP.hpp"
#include "Routes.hpp"

namespace Routes {
void read_from(HTTP::Response* const res, std::function<void(std::string_view const)> handler) {
	std::string storage;
	res
		->onAborted([res]() {
			HTTP::send_code_handler<HTTP::Status::BAD_REQUEST>(res);
		})
		->onData([res, storage = std::move(storage), handler = std::move(handler)](std::string_view const chunk, bool const last) mutable {
			storage.append(chunk.data(), chunk.size());
			if (last) {
				HTTP_EXCEPT_WRAP_BEGIN
				handler(storage);
				HTTP_EXCEPT_WRAP_END
			}
		});
}

std::string const splat_route = "/*";

ROUTES_CUSTOM_REGISTERER_IMPL(404_handler, app) {
	return app.any(splat_route, HTTP::send_code_handler<HTTP::Status::NOT_FOUND>);
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
