#include "Routes.hpp"

namespace Routes {
namespace users {
namespace username {

std::string const param_path = "/users/username/:username";

ROUTE_IMPL_NOEXCEPT(param_get, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get);
}
}  // namespace username
}  // namespace users
}  // namespace Routes
