#include <memory>
#include <tao/pq/connection.hpp>

#include "ORM/ConstraintException.hpp"
#include "ORM/User.hpp"

#include "threadlocal.hpp"
#include "util.hpp"

namespace ORM {

std::string const User::SQL_CREATE = "insert into users (username, password, display_name) values ($1, $2, $3)";
std::string const User::SQL_CREATE_GET_ID = "select id from users where username = $1";
std::string const User::SQL_FETCH_BY_ID = "select username, password, display_name from users where id = $1";
std::string const User::SQL_FETCH_BY_USERNAME = "select id, password, display_name from users where username = $1";
std::string const User::SQL_FETCH_BY_DISPLAY_NAME = "select id, username, password from users where display_name = $1";
std::string const User::SQL_UPDATE_DISPLAY_NAME_PASSWORD = "update users set display_name = $1, password = $2 where id = $3";
std::string const User::SQL_UPDATE_DISPLAY_NAME = "update users set display_name = $1 where id = $2";
std::string const User::SQL_UPDATE_PASSWORD = "update users set password = $1 where id = $1";
std::string const User::SQL_DELETE_BY_ID = "delete from users where id = $1";

char const* const User::CLASS_NAME = "User";
char const* const User::FIELD_NAME_USERNAME = "username";

User::User() {
	// zero-initialize/default-construct all members
}

User::User(std::string username, std::string const& password, std::string display_name)
	: m_username(std::move(username)), m_password(Hash::hash(password)), m_display_name(std::move(display_name)) {
	try {
		ThreadLocal::conn->execute(SQL_CREATE, m_username, m_password, m_display_name);
		auto const result = ThreadLocal::conn->execute(SQL_CREATE_GET_ID, m_username);
		assert(result.size() == 1);
		m_id = result[0][0].as<decltype(m_id)>();
	} catch (std::runtime_error const& e) {
		// harrumph
		if (strstr(e.what(), "duplicate key value violates unique constraint")) {
			throw ORM::ConstraintException{ CLASS_NAME, FIELD_NAME_USERNAME };
		} else {
			throw;
		}
	}
}

User::~User() {
	save();
}

void User::save() {
	if (password_dirty && display_name_dirty) {
		ThreadLocal::conn->execute(SQL_UPDATE_DISPLAY_NAME_PASSWORD, m_display_name, m_password, m_id);
	} else if (password_dirty && !display_name_dirty) {
		ThreadLocal::conn->execute(SQL_UPDATE_PASSWORD, m_password, m_id);
	} else if (display_name_dirty && !password_dirty) {
		ThreadLocal::conn->execute(SQL_UPDATE_DISPLAY_NAME, m_display_name, m_id);
	}
}

std::optional<User> User::get_by_id(id_t const id) {
	auto const results = ThreadLocal::conn->execute(SQL_FETCH_BY_ID, id);
	assert(results.size() <= 1);  // 0 or 1 entries
	if (results.size() == 1) {
		auto const& result = results[0];
		return User{
			id,
			result["username"].as<std::string>(),
			result["password"].as<tao::pq::binary>(),
			result["display_name"].as<std::string>(),
		};
	} else {
		return std::nullopt;
	}
}
std::optional<User> User::get_by_name(std::string username) {
	auto const results = ThreadLocal::conn->execute(SQL_FETCH_BY_USERNAME, username);
	assert(results.size() <= 1);  // 0 or 1 entries
	if (results.size() == 1) {
		auto const& result = results[0];
		return User{
			result["id"].as<id_t>(),
			std::move(username),
			result["password"].as<tao::pq::binary>(),
			result["display_name"].as<std::string>(),
		};
	} else {
		return std::nullopt;
	}
}
std::vector<User, PrivateAllocator<User>> User::get_by_display_name(std::string_view const display_name) {
	auto const results = ThreadLocal::conn->execute(SQL_FETCH_BY_DISPLAY_NAME, display_name);
	std::vector<User, PrivateAllocator<User>> ret;
	ret.reserve(results.size());
	for (auto const& row : results) {
		ret.emplace_back(row["id"].as<id_t>(), row["username"].as<std::string>(), row["password"].as<tao::pq::binary>(), std::string{ display_name });
	}
	return ret;
}
bool User::delete_by_id(id_t const id) {
	auto const results = ThreadLocal::conn->execute(SQL_DELETE_BY_ID, id);
	assert(results.has_rows_affected());
	return results.rows_affected() == 1;
}

std::optional<User::id_t> User::id_from_param(std::string_view const param) {
	auto const maybe_user_id = util::sv_to_number<ORM::User::id_t>(param);
	if (maybe_user_id.has_value()) {
		return maybe_user_id;
	} else if (param == Strings::SELF) {
		return SELF_ID;
	} else {
		return std::nullopt;
	}
}

}  // namespace ORM
