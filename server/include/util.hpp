#pragma once

#include <algorithm>
#include <charconv>
#include <msgpack.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <uWebSockets/App.h>

namespace Strings {
extern std::string const SELF;
extern std::string const SQL_GET_ROW_COUNT;
}  // namespace Strings

namespace util {

inline bool is_digit(char const c) {
	return c >= '0' && c <= '9';
}
inline bool is_digits(std::string const& s) {
	return std::all_of(std::cbegin(s), std::cend(s), is_digit);
}
inline bool is_digits(std::string_view const s) {
	return std::all_of(std::cbegin(s), std::cend(s), is_digit);
}

template <typename I>
std::optional<I> sv_to_number(std::string_view const sv) {
	I result;
	auto const [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), result);

	if (err == std::errc{} && ptr == sv.data() + sv.size()) {
		return result;
	} else {
		return std::nullopt;
	}
}

void write_sv_to_unpacker(msgpack::unpacker& unpacker, std::string_view const sv);

template <typename T, typename E, typename... EArgs>
T& unwrap(std::optional<T>& x_opt, EArgs... e_args) {
	if (!x_opt.has_value()) {
		throw E{ e_args... };
	}
	return *x_opt;
}
template <typename T, typename E, typename... EArgs>
T&& unwrap(std::optional<T>&& x_opt, EArgs... e_args) {
	if (!x_opt.has_value()) {
		throw E{ e_args... };
	}
	return std::move(*x_opt);
}

}  // namespace util

// TERMINATES!!
#define TODO(STR) \
	std::clog << "TODO " << STR << " (" << __FILE__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << ')' << std::endl; \
	std::terminate();

// uses clog because it's buffered
#define CERR_EXCEPTION(e) std::clog << __FILE__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << " exception: " << e.what() << std::endl
