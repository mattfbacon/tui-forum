#pragma once

#include <algorithm>
#include <charconv>
#include <mariadb/conncpp/SQLString.hpp>
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

std::string sqlstr_to_str(sql::SQLString const& str);
sql::SQLString str_to_sqlstr(std::string const& str);
sql::SQLString sv_to_sqlstr(std::string_view const sv);

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

// for the most part this is copy-pasted from the builtin adaptor for std::string
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
	namespace adaptor {
	template <>
	struct convert<sql::SQLString> {
		msgpack::object const& operator()(msgpack::object const& o, sql::SQLString& v) const {
			switch (o.type) {
				case msgpack::type::BIN:
					v.clear();
					v.append(o.via.bin.ptr, o.via.bin.size);
					break;
				case msgpack::type::STR:
					v.clear();
					v.append(o.via.str.ptr, o.via.str.size);
					break;
				default:
					throw msgpack::type_error{};
					break;
			}
			return o;
		}
	};
	template <>
	struct pack<sql::SQLString> {
		template <typename Stream>
		msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, sql::SQLString const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.pack_str(size);
			o.pack_str_body(v.c_str(), size);
			return o;
		}
	};
	template <>
	struct object<sql::SQLString> {
		void operator()(msgpack::object& o, sql::SQLString const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.type = msgpack::type::STR;
			o.via.str.ptr = v.c_str();
			o.via.str.size = size;
		}
	};
	template <>
	struct object_with_zone<sql::SQLString> {
		void operator()(msgpack::object::with_zone& o, sql::SQLString const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.type = msgpack::type::STR;
			char* const ptr = reinterpret_cast<char*>(o.zone.allocate_align(size, MSGPACK_ZONE_ALIGNOF(char)));
			o.via.str.ptr = ptr;
			o.via.str.size = size;
			std::copy(std::cbegin(v), std::cend(v), ptr);
		}
	};
	}  // namespace adaptor
}
}  // namespace msgpack

// uses clog because it's buffered
#define CERR_EXCEPTION(e) std::clog << __FILE__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << " exception: " << e.what() << std::endl
