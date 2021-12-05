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
	MSGPACK_DEFINE_MAP(password, display_name)
};
}  // namespace

namespace Routes {
namespace users {
namespace id {

std::string const param_path = "/users/id/:id";

ORM::User resolve_user_parameter(uWS::HttpRequest* const req, unsigned short param_index, bool* resolved_with_self = nullptr) {
	auto const user_id = HTTP::unwrap(ORM::User::id_from_param(req->getParameter(param_index)), HTTP::Status::NOT_FOUND);
	if (user_id == ORM::User::SELF_ID) {
		std::string bearer = HTTP::unwrap(HTTP::resolve_bearer(req), HTTP::Status::UNAUTHORIZED);
		if (resolved_with_self) {
			*resolved_with_self = true;
		}
		return HTTP::unwrap(ORM::User::get_by_name(bearer), HTTP::Status::NOT_FOUND);
	} else {
		if (resolved_with_self) {
			*resolved_with_self = false;
		}
		return HTTP::unwrap(ORM::User::get_by_id(user_id), HTTP::Status::NOT_FOUND);
	}
}

ROUTE_IMPL_BEGIN(param_get, res, req)
	auto const user = resolve_user_parameter(req, 0);
	HTTP::ResponseWrapper wrapper{ res };
	msgpack::packer packer{ wrapper };
	packer.pack(user);
	res->end();
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(param_patch, res, req)
	auto user = HTTP::unwrap(ORM::User::get_by_name(HTTP::unwrap(HTTP::resolve_bearer(req), HTTP::Status::UNAUTHORIZED)), HTTP::Status::NOT_FOUND);
	// if an explicit ID was provided and that ID doesn't match the ID of the logged-in user, deny access
	if (auto const provided_id = ORM::User::id_from_param(req->getParameter(0)); provided_id != ORM::User::SELF_ID && user.id() != provided_id) {
		return HTTP::send_code_handler(*res, HTTP::Status::FORBIDDEN);
	}
	read_from(res, [res, &user](std::string_view const msgpack_data) mutable {
		// object handle must stay in scope while we work on the object
		msgpack::object_handle oh = msgpack::unpack(msgpack_data.data(), msgpack_data.size());
		msgpack::object const& obj = *oh;
		UserPatch patch;
		obj.convert(patch);  // FIXME: does this throw??? no documentation...
		if (patch.password.has_value()) {
			user.set_password(BCrypt::generateHash(*patch.password));
		}
		if (patch.display_name.has_value()) {
			user.set_display_name(*patch.display_name);
		}
		user.save();
		HTTP::ResponseWrapper wrapper{ res };
		msgpack::packer packer{ wrapper };
		packer.pack(user);
		res->end();
	});
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(param_delete, res, req)
	auto const user = HTTP::unwrap(ORM::User::get_by_name(HTTP::unwrap(HTTP::resolve_bearer(req), HTTP::Status::UNAUTHORIZED)), HTTP::Status::NOT_FOUND);
	// if an explicit ID was provided and that ID doesn't match the ID of the logged-in user, deny access
	if (auto const provided_id = ORM::User::id_from_param(req->getParameter(0)); provided_id != ORM::User::SELF_ID && user.id() != provided_id) {
		return HTTP::send_code_handler(*res, HTTP::Status::FORBIDDEN);
	}
	auto const deleted = ORM::User::delete_by_id(user.id());
	return HTTP::send_code_handler(*res, deleted ? HTTP::Status::NO_CONTENT : HTTP::Status::NOT_FOUND);
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	return app.any(param_path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(param_path, param_get).patch(param_path, param_patch).del(param_path, param_delete);
}

}  // namespace id
}  // namespace users
}  // namespace Routes
