#include <bcrypt/BCrypt.hpp>
#include <iostream>
#include <optional>
#include <unordered_map>

#include "Config.hpp"
#include "HTTP.hpp"
#include "ORM/User.hpp"
#include "Routes.hpp"

#include "util.hpp"

namespace {
struct UserPatch {
	std::optional<std::string> password;
	std::optional<std::string> display_name;
	MSGPACK_DEFINE(password, display_name)
};
}  // namespace

namespace Routes {
namespace users {
namespace id {

std::string const param_path = "/users/id/:id";

ROUTE_IMPL_BEGIN(param_get, res, req)
#warning "No self!"
// TODO: add self
auto const user_id = HTTP::unwrap(ORM::User::id_from_param(req->getParameter(0), /* self here */ 0), HTTP::Status::NOT_FOUND);
auto const user = HTTP::unwrap(ORM::User::get_by_id(user_id), HTTP::Status::NOT_FOUND);
ResponseWrapper wrapper{ res };
msgpack::packer packer{ wrapper };
packer.pack(user);
res->end();
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(param_patch, res, req)
auto const user_id = HTTP::unwrap(ORM::User::id_from_param(req->getParameter(0), /* self here */ 0), HTTP::Status::NOT_FOUND);
auto user = HTTP::unwrap(ORM::User::get_by_id(user_id), HTTP::Status::NOT_FOUND);
// TODO when auth is added: check that id is the user's id, otherwise send 403
#warning "No auth!"
std::string msgpack_data;
res
	->onAborted([res]() {
		res->writeStatus(HTTP_STATUS(HTTP::Status::BAD_REQUEST));
	})
	->onData([res, msgpack_data = std::move(msgpack_data), user = std::move(user)](std::string_view const data, bool const last) mutable {
		msgpack_data.append(data.data(), data.size());
		if (last) {
			HTTP_EXCEPT_WRAP_BEGIN
			// object handle must stay in scope while we work on the object
			msgpack::object_handle oh = msgpack::unpack(msgpack_data.data(), msgpack_data.size());
			msgpack::object obj = *oh;
			UserPatch patch;
			obj.convert(patch);  // FIXME: does this throw??? no documentation...
			if (patch.password.has_value()) {
				user.set_password(str_to_sqlstr(BCrypt::generateHash(*patch.password)));
			}
			if (patch.display_name.has_value()) {
				user.set_display_name(str_to_sqlstr(*patch.display_name));
			}
			user.save();
			ResponseWrapper wrapper{ res };
			msgpack::packer packer{ wrapper };
			packer.pack(user);
			res->end();
			HTTP_EXCEPT_WRAP_END
		}
	});
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(param_delete, res, req)
// TODO when auth is added: add self id; check that id is the user's id, otherwise send 403
#warning "No auth! No self!"
auto const user_id = HTTP::unwrap(ORM::User::id_from_param(req->getParameter(0), 0), HTTP::Status::NOT_FOUND);
auto const deleted = ORM::User::delete_by_id(user_id);
if (deleted) {
	return send_code_handler(*res, HTTP::Status::NO_CONTENT);
} else {
	throw HTTP::StatusException{ HTTP::Status::NOT_FOUND };
}
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get).patch(param_path, param_patch).del(param_path, param_delete);
}

}  // namespace id
}  // namespace users
}  // namespace Routes