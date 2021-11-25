#pragma once

#include <bcrypt/bcrypt.h>
#include <msgpack.hpp>
#include <msgpack/adaptor/int.hpp>
#include <optional>
#include <string>
#include <tao/pq/binary.hpp>
#include <vector>

#include "Hash.hpp"
#include "PrivateAllocator.hpp"

// #include "ORM/Post.hpp"

namespace ORM {

class User final {
public:
	using id_t = uint64_t;
	static id_t constexpr SELF_ID = 0;
	static size_t constexpr PASSWORD_HASH_LENGTH = BCRYPT_HASHSIZE;
	friend struct PrivateAllocator<User>;
public:
	User();
	// create a user
	User(std::string username, std::string const& password, std::string display_name);
	User(User const&) = delete;  // can't copy since id is unique
	User(User&&) = default;
	~User();
	inline auto id() const {
		return m_id;
	}
	inline auto const& username() const {
		return m_username;
	}
	inline auto const& password() const {
		return m_password;
	}
	void set_password(std::string_view const value) {
		m_password = Hash::hash(value);
		password_dirty = true;
	}
	bool check_password(std::string_view const value) const {
		return Hash::check(value, m_password);
	}

	inline auto const& display_name() const {
		return m_display_name;
	}
	inline void set_display_name(std::string value) {
		m_display_name = std::move(value);
		display_name_dirty = true;
	}
public:
	void save();
	// std::vector<Post> get_posts();
	static std::optional<User> get_by_id(id_t id);
	static std::optional<User> get_by_name(std::string_view const username);
	static std::vector<User, PrivateAllocator<User>> get_by_display_name(std::string_view const display_name);
	static bool delete_by_id(id_t id);
	static std::optional<id_t> id_from_param(std::string_view const param);
protected:
	User(id_t id, std::string username, tao::pq::binary password, std::string display_name)
		: m_id(std::move(id)), m_username(std::move(username)), m_password(std::move(password)), m_display_name(std::move(display_name)) {
		assert(m_password.size() == PASSWORD_HASH_LENGTH);
	}
protected:
	// even though it should be, it can't be const due to msgpack weirdness
	id_t m_id;
	std::string m_username;
	tao::pq::binary m_password;
	std::string m_display_name;
	bool password_dirty = false;
	bool display_name_dirty = false;
protected:
	static std::string const SQL_FETCH_BY_ID;
	static std::string const SQL_FETCH_BY_USERNAME;
	static std::string const SQL_FETCH_BY_DISPLAY_NAME;
	static std::string const SQL_UPDATE_DISPLAY_NAME_PASSWORD;
	static std::string const SQL_UPDATE_DISPLAY_NAME;
	static std::string const SQL_UPDATE_PASSWORD;
	static std::string const SQL_DELETE_BY_ID;
	static std::string const SQL_CREATE;
	static std::string const SQL_CREATE_GET_ID;
protected:
	static char const* const CLASS_NAME;
	static char const* const FIELD_NAME_USERNAME;
public:
	MSGPACK_DEFINE_MAP(MSGPACK_NVP("id", m_id), MSGPACK_NVP("username", m_username), MSGPACK_NVP("display_name", m_display_name))
};

}  // namespace ORM
