#include "ORM/User.hpp"
#include "Routes.hpp"

namespace Routes {
namespace users {
namespace username {

std::string const param_path = "/users/username/:username";

ROUTE_IMPL_BEGIN(param_get, res, req)
auto const param_username = req -> getParameter(0);
auto const user = ORM::User::get_by_name(param_username);
if (!user.has_value()) {
	throw HTTP::StatusException{ HTTP::Status::NOT_FOUND };
}
res->writeStatus(HTTP_STATUS(HTTP::Status::OK));
HTTP::ResponseWrapper wrapper{ res };
msgpack::packer packer{ wrapper };
packer.pack(user);
res->end();
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get);
}
}  // namespace username
}  // namespace users
}  // namespace Routes
