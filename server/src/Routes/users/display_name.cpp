#include "ORM/User.hpp"
#include "Routes.hpp"

namespace Routes {
namespace users {
namespace display_name {

std::string const param_path = "/users/display_name/:display_name";

ROUTE_IMPL_BEGIN(param_get, res, req)
auto const param_display_name_ = HTTP::decode_uri<std::string>(req->getParameter(0));
auto const param_display_name = HTTP::unwrap<std::string const>(std::move(param_display_name_), HTTP::Status::BAD_REQUEST);

auto const users = ORM::User::get_by_display_name(param_display_name);
res->writeStatus(HTTP_STATUS(HTTP::Status::OK));
HTTP::ResponseWrapper wrapper{ res };
msgpack::packer packer{ wrapper };
packer.pack(users);
res->end();
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get);
}
}  // namespace display_name
}  // namespace users
}  // namespace Routes
