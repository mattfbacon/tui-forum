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

namespace Routes {
using Request = uWS::HttpRequest;
using Response = uWS::HttpResponse<WebConfig::USE_SSL>;
using Route = void(Response* res, Request* req);
#define REGISTERER(NAME) ROUTES_CUSTOM_REGISTERER_IMPL(NAME, )

namespace users {

namespace session {
// get existing session
extern Route get __attribute__((nonnull(1, 2)));
// create a new session
extern Route post;
// delete the current session (log out)
extern Route delete_;
}  // namespace session

namespace id {
// get user by id
extern Route param_get;
// edit user by id
extern Route param_patch;
// delete user by id
extern Route param_delete;

REGISTERER(all);
}  // namespace id

namespace username {
// get user by username
extern Route param_get;
}  // namespace username

namespace displayname {
// get user(s) by display name
extern Route param_get;
}  // namespace displayname

// create a new user
extern Route post;
}  // namespace users

namespace posts {
// search posts
extern Route get;
// get post by id
extern Route param_get;
// create new post
extern Route post;
// delete post by id
extern Route param_delete;
// edit post by id
extern Route param_patch;
}  // namespace posts

void send_code_handler(Response& res, HTTP::Status::code_t const code);
template <HTTP::Status::code_t code>
__attribute__((nonnull(1))) void send_code_handler(Response* const res, Request* const = nullptr) {
	send_code_handler(*res, code);
}

REGISTERER(404_handler);
REGISTERER(all);

}  // namespace Routes

#undef REGISTERER
