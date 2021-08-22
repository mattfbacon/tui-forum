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
ORM::User::id_t user_id_from_param(std::string_view const param) noexcept(false) {
	auto const user_id_ = sv_to_number<ORM::User::id_t>(param);
	if (user_id_.has_value()) {
		return *user_id_;
	} else if (param == Strings::SELF) {
		// TODO when auth is added: get self id and assign to user_id; throw UNAUTHORIZED if not logged in
		TODO("self");
	} else {
		throw HTTP::StatusException{ HTTP::Status::NOT_FOUND };
	}
}
}  // namespace

namespace Routes {
namespace users {
namespace id {

std::string const path = "/users/id/:id";

void param_get(Response* const res, Request* const req) {
	try {
		auto const user = HTTP::unwrap(ORM::User::get_by_id(user_id_from_param(req->getParameter(0))), HTTP::Status::NOT_FOUND);
		ResponseWrapper wrapper{ res };
		msgpack::packer packer{ wrapper };
		packer.pack(user);
		res->end();
	} catch (HTTP::StatusException const& e) {
		return send_code_handler(*res, e.code);
	} catch (...) {
		return send_code_handler<HTTP::Status::INTERNAL_SERVER_ERROR>(res);
	}
}

void dumb_abort_handler() {
	std::clog << __FILE__ << ' ' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << std::endl;
}

void param_patch(Response* const res, Request* const req) {
	auto user = HTTP::unwrap(ORM::User::get_by_id(user_id_from_param(req->getParameter(0))), HTTP::Status::NOT_FOUND);
	// TODO when auth is added: check that id is the user's id, otherwise send 403
	std::string msgpack_data;
	res->onAborted(dumb_abort_handler)->onData([res, msgpack_data = std::move(msgpack_data), user = std::move(user)](std::string_view const data, bool const last) mutable {
		msgpack_data.append(data.data(), data.size());
		if (last) {
			try {
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
			} catch (msgpack::parse_error const&) {
				return send_code_handler<HTTP::Status::BAD_REQUEST>(res);
			} catch (msgpack::type_error const&) {
				return send_code_handler<HTTP::Status::BAD_REQUEST>(res);
			} catch (msgpack::unpack_error const&) {
				return send_code_handler<HTTP::Status::BAD_REQUEST>(res);
			} catch (HTTP::StatusException const& e) {
				return send_code_handler(*res, e.code);
			} catch (...) {
				return send_code_handler<HTTP::Status::INTERNAL_SERVER_ERROR>(res);
			}
		}
	});
}

uWS::App&& register_all(uWS::App&& app) {
	return app.any(path, send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(path, param_get).patch(path, param_patch) /* TODO .delete("/user/id/:id", param_delete) */;
}

}  // namespace id
}  // namespace users
}  // namespace Routes
