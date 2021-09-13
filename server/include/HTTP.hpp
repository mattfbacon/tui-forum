#pragma once

#include <optional>
#include <string>
#include <string_view>
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

template <typename T>
concept IStringPiecewise = requires(T v, size_t reserve_size, char append_char) {
	v.reserve(reserve_size);
	v += append_char;
};

// this code is from inside uWebSockets: https://github.com/uNetworking/uWebSockets/blob/5f0065825a869fd31f5cf52a51b218f4402bf380/src/QueryParser.h#L62
template <typename StringType>
std::optional<StringType> decode_uri(std::string_view const encoded) requires IStringPiecewise<StringType> {
	StringType out_data;
	size_t const in_data_size = encoded.size();
	out_data.reserve(in_data_size);  // max length
	for (size_t i = 0; i < in_data_size; i++) {
		if (char const current_char = encoded[i]; current_char == '%') {
			if (i + 2 >= in_data_size) {
				return std::nullopt;
			}

			int hex1 = (int)encoded[i + 1] - '0';
			if (hex1 > 9) {
				hex1 &= 223;
				hex1 -= 7;
			}

			int hex2 = (int)encoded[i + 2] - '0';
			if (hex2 > 9) {
				hex2 &= 223;
				hex2 -= 7;
			}

			out_data += (unsigned char)(hex1 * 16 + hex2);
			i += 2;
		} else {
			out_data += current_char;
		}
	}
	return { out_data };
}

}  // namespace HTTP

#define HTTP_STATUS(VAL) HTTP::Status::strings.at(VAL)
