#include "HTTP.hpp"
#include "ORM/User.hpp"
#include "Routes.hpp"

namespace Routes {
namespace users {

std::string const path = "/users";

namespace {
struct UserCreationData {
	std::string username;
	std::string display_name;
	std::string password;
	MSGPACK_DEFINE(username, display_name, password)
};
}  // namespace

ROUTE_IMPL_BEGIN(post, res, req)
// TODO
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	// clang-format off
	return
		display_name::register_all(
		username::register_all(
		id::register_all(
		session::register_all(
			app.any(path, send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).post(path, post)
		)
		)
		)
		)
		;
	// clang-format on
}
}  // namespace users
}  // namespace Routes
