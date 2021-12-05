#pragma once

#include <uWebSockets/App.h>

namespace uWS {
class HttpRequest;
template <bool SSL>
class HttpResponse;
}  // namespace uWS

#include "Config.hpp"
#include "HTTP.hpp"

#define ROUTES_CUSTOM_REGISTERER_IMPL(NAME, PARAM) uWS::App&& register_##NAME(uWS::App&& PARAM)
#define ROUTES_REGISTERER_IMPL(PARAM) ROUTES_CUSTOM_REGISTERER_IMPL(all, PARAM)
#define ROUTE_IMPL_NOEXCEPT(NAME, PARAM_RES, PARAM_REQ) void NAME(Response* const PARAM_RES, Request* const PARAM_REQ)
#define HTTP_EXCEPT_WRAP_BEGIN try {
#define HTTP_EXCEPT_WRAP_END_MINIMAL \
	} \
	catch (HTTP::StatusException const& e) { \
		return HTTP::send_code_handler(*res, e.code); \
	} \
	catch (std::exception const& e) { \
		CERR_EXCEPTION(e); \
		return HTTP::send_code_handler(*res, HTTP::Status::INTERNAL_SERVER_ERROR); \
	}
#define HTTP_EXCEPT_WRAP_END \
	} \
	catch (msgpack::parse_error const&) { \
		return HTTP::send_code_handler<HTTP::Status::BAD_REQUEST>(res); \
	} \
	catch (msgpack::type_error const&) { \
		return HTTP::send_code_handler<HTTP::Status::BAD_REQUEST>(res); \
	} \
	catch (msgpack::unpack_error const&) { \
		return HTTP::send_code_handler<HTTP::Status::BAD_REQUEST>(res); \
		HTTP_EXCEPT_WRAP_END_MINIMAL
#define ROUTE_IMPL_BEGIN(NAME, PARAM_RES, PARAM_REQ) \
	ROUTE_IMPL_NOEXCEPT(NAME, PARAM_RES, PARAM_REQ) { \
		HTTP_EXCEPT_WRAP_BEGIN
#define ROUTE_IMPL_END \
	HTTP_EXCEPT_WRAP_END \
	}

namespace Routes {
using HTTP::Request;
using HTTP::Response;
using Route = void(Response* res, Request* req);
#define REGISTERER(NAME) ROUTES_CUSTOM_REGISTERER_IMPL(NAME, )
#define ROUTE(NAME) extern Route NAME __attribute__((nonnull(1, 2)));

void read_from(Response* res, std::function<void(std::string_view const)> handler);

namespace users {

namespace session {
// get existing session
ROUTE(get);
// create a new session
ROUTE(post);
// delete the current session (log out)
ROUTE(delete_);

REGISTERER(all);
}  // namespace session

namespace id {
// get user by id
ROUTE(param_get);
// edit user by id
ROUTE(param_patch);
// delete user by id
ROUTE(param_delete);

REGISTERER(all);
}  // namespace id

namespace username {
// get user by username
ROUTE(param_get);

REGISTERER(all);
}  // namespace username

namespace display_name {
// get user(s) by display name
ROUTE(param_get);

REGISTERER(all);
}  // namespace display_name

// create a new user
ROUTE(post);

REGISTERER(all);
}  // namespace users

namespace posts {
// search posts
ROUTE(get);
// get post by id
ROUTE(param_get);
// create new post
ROUTE(post);
// delete post by id
ROUTE(param_delete);
// edit post by id
ROUTE(param_patch);

REGISTERER(all);
}  // namespace posts

REGISTERER(404_handler);
REGISTERER(all);

}  // namespace Routes

#undef REGISTERER
#undef ROUTE
