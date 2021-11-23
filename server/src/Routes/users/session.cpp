#include <recollect.hpp>

#include "Config.hpp"
#include "ORM/User.hpp"
#include "Routes.hpp"

#include "threadlocal.hpp"
#include "util.hpp"

namespace Routes {
namespace users {
namespace session {

namespace {

struct UserForTokenResponse {
	std::string id;
	MSGPACK_DEFINE_MAP(id);
};

struct SessionPostData {
	std::string username;
	std::string password;
	MSGPACK_DEFINE_MAP(username, password);
};

struct SessionCreationData {
	std::string token;
	MSGPACK_DEFINE_MAP(token);
};

template <bool CheckForUniqueness = true>
std::string create_token() {
	std::string ret;
	ret.resize(SecurityConfig::TOKEN_SIZE);
	do {
		std::mt19937 rng{ ThreadLocal::random_device() };
		// `char` as distribution type is UB, so use `short` and cast
		// ref: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
		std::uniform_int_distribution<short> dist{ static_cast<short>('a'), static_cast<short>('a' + 16) };
		for (size_t i = 0; i < SecurityConfig::TOKEN_SIZE; i++) {
			ret[i] = static_cast<char>(dist(rng));
		}
	} while (!CheckForUniqueness || ThreadLocal::cache->has(ret));
	return ret;
}

}  // namespace

std::string const path = "/users/session";

ROUTE_IMPL_BEGIN(get, res, req)
	auto const token_sv = req->getHeader("bearer");
	if (token_sv.empty()) {
		return HTTP::send_code_handler(*res, HTTP::Status::NOT_FOUND);
	}
	auto const user_for_token = ThreadLocal::cache->get(token_sv);
	if (!user_for_token.has_value()) {
		return HTTP::send_code_handler(*res, HTTP::Status::NOT_FOUND);
	}

	HTTP::ResponseWrapper wrapper{ res };
	msgpack::packer packer{ wrapper };
	packer.pack(UserForTokenResponse{ std::string{ user_for_token.sv() } });
	res->end();
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(post, res, req)
	read_from(res, [req, res](std::string_view const msgpack_data) {
		// input
		// unpack data
		auto handle = msgpack::unpack(msgpack_data.data(), msgpack_data.size());
		auto const& obj = *handle;
		SessionPostData input_data;
		obj.convert(input_data);
		// check credentials
		auto const user = ORM::User::get_by_name(input_data.username);
		if (!user.has_value() || !user->check_password(input_data.password)) {
			return HTTP::send_code_handler(*res, HTTP::Status::FORBIDDEN);
		}
		// output
		// create token and insert into cache
		SessionCreationData output_data{ create_token() };
		ThreadLocal::cache->set(std::string_view{ reinterpret_cast<char const*>(output_data.token.c_str()), output_data.token.size() }, input_data.username);
		// send token
		res->writeStatus(HTTP_STATUS(HTTP::Status::OK));
		HTTP::ResponseWrapper wrapper{ res };
		msgpack::packer packer{ wrapper };
		packer.pack(output_data);
		res->end();
	});
ROUTE_IMPL_END

ROUTE_IMPL_BEGIN(delete_, res, req)
	auto const token_sv = req->getHeader("bearer");
	if (token_sv.empty()) {
		return HTTP::send_code_handler(*res, HTTP::Status::NOT_FOUND);
	}
	bool const was_deleted = ThreadLocal::cache->delete_(token_sv);
	return HTTP::send_code_handler(*res, was_deleted ? HTTP::Status::NO_CONTENT : HTTP::Status::NOT_FOUND);
ROUTE_IMPL_END

ROUTES_REGISTERER_IMPL(app) {
	return app.any(path, HTTP::send_code_handler<HTTP::Status::METHOD_NOT_ALLOWED>).get(path, get).post(path, post).del(path, delete_);
}
}  // namespace session
}  // namespace users
}  // namespace Routes
