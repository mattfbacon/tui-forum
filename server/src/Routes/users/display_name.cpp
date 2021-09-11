#include "Routes.hpp"

namespace Routes {
namespace users {
namespace display_name {

std::string const param_path = "/users/display_name/:display_name";

ROUTE_IMPL_NOEXCEPT(param_get, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get);
}
}  // namespace display_name
}  // namespace users
}  // namespace Routes
