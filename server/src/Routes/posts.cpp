#include "Routes.hpp"

namespace Routes {
namespace posts {

std::string const path = "/posts";
std::string const param_path = "/posts/:id";

ROUTE_IMPL_NOEXCEPT(get, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(param_get, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(post, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(param_delete, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}
ROUTE_IMPL_NOEXCEPT(param_patch, res, req) {
	(void)res;
	(void)req;
#warning "TODO"
}

ROUTES_REGISTERER_IMPL(app) {
	return app.any(path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>)
		.any(param_path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>)
		.get(path, get)
		.get(param_path, param_get)
		.post(path, post)
		.del(param_path, param_delete)
		.patch(param_path, param_patch);
}
}  // namespace posts
}  // namespace Routes
