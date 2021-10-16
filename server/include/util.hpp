#pragma once

#include <algorithm>
#include <charconv>
#include <msgpack.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <sys/random.h>
#include <tao/pq/binary.hpp>
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

template <typename T, typename Item>
concept StandardBuffer = requires(T& buf) {
	{ buf.size() } -> std::convertible_to<std::size_t>;
	{ buf.data() } -> std::same_as<Item*>;
};

template <typename Buf>
void get_random_bytes(Buf& buf) requires StandardBuffer<Buf, std::byte> {
	auto* data_ptr = buf.data();
	size_t buf_size = buf.size();
	while (buf_size > 0) {
		auto const bytes_read = getrandom(data_ptr, buf_size, 0);
		if (bytes_read == -1) {
			throw std::system_error{ errno, std::system_category() };
		}
		data_ptr += bytes_read;
		buf_size -= bytes_read;
	}
}
}  // namespace util

// TERMINATES!!
#define TODO(STR) \
	std::clog << "TODO " << STR << " (" << __FILE__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << ')' << std::endl; \
	std::terminate();

// uses clog because it's buffered
#define CERR_EXCEPTION(e) std::clog << __FILE__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << " exception: " << e.what() << std::endl

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
	namespace adaptor {
	template <>
	struct convert<tao::pq::binary> {
		msgpack::object const& operator()(msgpack::object const& o, tao::pq::binary& v) const {
			switch (o.type) {
				case msgpack::type::BIN:
					v.erase();
					v.append((std::byte*)o.via.bin.ptr, o.via.bin.size);
					break;
				case msgpack::type::STR:
					v.erase();
					v.append((std::byte*)o.via.str.ptr, o.via.str.size);
					break;
				default:
					throw msgpack::type_error{};
					break;
			}
			return o;
		}
	};
	template <>
	struct pack<tao::pq::binary> {
		template <typename Stream>
		msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, tao::pq::binary const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.pack_bin(size);
			o.pack_bin_body((char*)v.c_str(), size);
			return o;
		}
	};
	template <>
	struct object<tao::pq::binary> {
		void operator()(msgpack::object& o, tao::pq::binary const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.type = msgpack::type::BIN;
			o.via.bin.ptr = (char*)v.c_str();
			o.via.bin.size = size;
		}
	};
	template <>
	struct object_with_zone<tao::pq::binary> {
		void operator()(msgpack::object::with_zone& o, tao::pq::binary const& v) const {
			auto const size = checked_get_container_size(v.size());
			o.type = msgpack::type::BIN;
			char* const ptr = reinterpret_cast<char*>(o.zone.allocate_align(size, MSGPACK_ZONE_ALIGNOF(char)));
			o.via.bin.ptr = ptr;
			o.via.bin.size = size;
			std::copy((char*)v.data(), (char*)(v.data() + v.size()), ptr);
		}
	};
	}  // namespace adaptor
}
}  // namespace msgpack
