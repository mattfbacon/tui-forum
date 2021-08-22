#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "util.hpp"

namespace HTTP {
namespace Status {
using code_t = int;
constexpr code_t const OK = 200;
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
	return ::unwrap<T, StatusException, Status::code_t>(x_opt, status_code);
}
template <typename T>
T&& unwrap(std::optional<T>&& x_opt, HTTP::Status::code_t const status_code) {
	return ::unwrap<T, StatusException, Status::code_t>(std::forward<std::optional<T>>(x_opt), status_code);
}
}  // namespace HTTP

#define HTTP_STATUS(VAL) HTTP::Status::strings.at(VAL)
