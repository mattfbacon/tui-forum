#include "Routes.hpp"

namespace Routes {
namespace users {
namespace session {

std::string const path = "/users/session";

ROUTE_IMPL_NOEXCEPT(get, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(post, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(delete_, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}

ROUTES_REGISTERER_IMPL(app) {
	return app.any(path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(path, get).post(path, post).del(path, delete_);
}
}  // namespace session
}  // namespace users
}  // namespace Routes
