#pragma once

#include <msgpack.hpp>
#include <msgpack/adaptor/int.hpp>
#include <optional>
#include <string>
#include <vector>

// #include "ORM/Post.hpp"

namespace ORM {

class User final {
public:
	using id_t = uint64_t;
public:
	User();
	// create a user
	User(std::string username, std::string const& password, std::string display_name);
	User(User const&) = delete;  // can't copy since id is unique
	User(User&&) = default;
	~User();
	inline id_t id() const {
		return m_id;
	}
	inline sql::SQLString const& username() const {
		return m_username;
	}
	inline sql::SQLString const& password() const {
		return m_password;
	}
	inline void set_password(sql::SQLString value) {
		assert(value.size() == 64);
		m_password = std::move(value);
		password_dirty = true;
	}
	inline sql::SQLString const& display_name() const {
		return m_display_name;
	}
	inline void set_display_name(sql::SQLString value) {
		m_display_name = std::move(value);
		display_name_dirty = true;
	}
public:
	void save();
	// std::vector<Post> get_posts();
	static std::optional<User> get_by_id(id_t id);
	static std::optional<User> get_by_name(sql::SQLString const& username);
	static std::vector<User> get_by_display_name(sql::SQLString const& display_name);
	static bool delete_by_id(id_t id);
	static std::optional<id_t> id_from_param(std::string_view const param, ORM::User::id_t const self_id);
protected:
	User(id_t id, sql::SQLString username, sql::SQLString password, sql::SQLString display_name);
protected:
	// even though it should be, it can't be const due to msgpack weirdness
	id_t m_id;
	sql::SQLString m_username;
	sql::SQLString m_password;
	sql::SQLString m_display_name;
	bool password_dirty = false;
	bool display_name_dirty = false;
protected:
	// stored as std::string to force runtime copy since SQLStrings aren't thread-safe (even when const)
	static std::string const SQL_FETCH_BY_ID;
	static std::string const SQL_FETCH_BY_USERNAME;
	static std::string const SQL_FETCH_BY_DISPLAY_NAME;
	static std::string const SQL_UPDATE_DISPLAY_NAME_PASSWORD;
	static std::string const SQL_UPDATE_DISPLAY_NAME;
	static std::string const SQL_UPDATE_PASSWORD;
	static std::string const SQL_DELETE_BY_ID;
	static std::string const SQL_CREATE;
public:
	MSGPACK_DEFINE(m_id, m_username, m_display_name)
};

}  // namespace ORM
