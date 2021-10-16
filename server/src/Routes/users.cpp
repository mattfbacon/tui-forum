#include "HTTP.hpp"
#include "ORM/ConstraintException.hpp"
#include "ORM/User.hpp"
#include "Routes.hpp"

namespace Routes {
namespace users {

std::string const path = "/users";

namespace {
struct UserCreationData {
	std::string username;
	std::string password;
	std::string display_name;
	MSGPACK_DEFINE_MAP(username, password, display_name)
};
}  // namespace

ROUTE_IMPL_BEGIN(post, res, )
	read_from(res, [res](std::string_view msgpack_data) {
		try {
			// object handle must stay in scope while we work on the object
			msgpack::object_handle oh = msgpack::unpack(msgpack_data.data(), msgpack_data.size());
			msgpack::object const& obj = *oh;
			UserCreationData data;
			obj.convert(data);
			// create user
			ORM::User user{ data.username, data.password, data.display_name };
			res->writeStatus(HTTP_STATUS(HTTP::Status::CREATED));
			HTTP::ResponseWrapper wrapper{ res };
			msgpack::packer packer{ wrapper };
			packer.pack(user);
			res->end();
		} catch (ORM::ConstraintException const& e) {
			if (e.matches("User", "username")) {
				res->writeStatus(HTTP_STATUS(HTTP::Status::UNPROCESSABLE_ENTITY));
				res->write("Username taken");
				res->end();
			}
		}
	});
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	// clang-format off
	return
		display_name::register_all(
		username::register_all(
		id::register_all(
		session::register_all(
			app.any(path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).post(path, post)
		)
		)
		)
		)
		;
	// clang-format on
}
}  // namespace users
}  // namespace Routes
