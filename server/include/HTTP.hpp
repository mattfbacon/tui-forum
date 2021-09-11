#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "Config.hpp"

#include "util.hpp"

namespace HTTP {

// this is necessary because uWS::HttpResponse does not have a write signature that is compatible with standard streams, instead taking a std::string_view. Hence, this wrapper simply constructs one from the data and size (exposing the proper signature), and passes it to the HttpResponse write function.
template <bool SSL>
class ResponseWrapper {
public:
	// non-owning pointer
	ResponseWrapper(uWS::HttpResponse<SSL>* res) : res(res) {}
	ResponseWrapper& write(char const* const data, size_t const size) {
		res->write(std::string_view{ data, size });
		return *this;
	}
	uWS::HttpResponse<SSL>* res;
};

using Request = uWS::HttpRequest;
using Response = uWS::HttpResponse<WebConfig::USE_SSL>;

namespace Status {
using code_t = int;
constexpr code_t const OK = 200;
constexpr code_t const CREATED = 201;
constexpr code_t const NO_CONTENT = 204;
constexpr code_t const BAD_REQUEST = 400;
constexpr code_t const UNAUTHORIZED = 401;
constexpr code_t const FORBIDDEN = 403;
constexpr code_t const NOT_FOUND = 404;
constexpr code_t const METHOD_NOT_ALLOWED = 405;
constexpr code_t const I_M_A_TEAPOT = 418;
constexpr code_t const UNPROCESSABLE_ENTITY = 422;
constexpr code_t const INTERNAL_SERVER_ERROR = 500;
extern std::unordered_map<code_t, std::string> const strings;
}  // namespace Status

class StatusException : public std::exception {
public:
	StatusException(Status::code_t const code) : std::exception{}, code(code) {}
	char const* what() const noexcept override;
	Status::code_t const code;
};
template <typename T>
T& unwrap(std::optional<T>& x_opt, HTTP::Status::code_t const status_code) {
	return util::unwrap<T, StatusException, Status::code_t>(x_opt, status_code);
}
template <typename T>
T&& unwrap(std::optional<T>&& x_opt, HTTP::Status::code_t const status_code) {
	return util::unwrap<T, StatusException, Status::code_t>(std::forward<std::optional<T>>(x_opt), status_code);
}

void send_code_handler(Response& res, HTTP::Status::code_t const code);
template <HTTP::Status::code_t code>
void __attribute__((nonnull(1))) send_code_handler(Response* const res, Request* const = nullptr) {
	send_code_handler(*res, code);
}

}  // namespace HTTP

#define HTTP_STATUS(VAL) HTTP::Status::strings.at(VAL)
