#include <mariadb/conncpp/PreparedStatement.hpp>
#include <mariadb/conncpp/ResultSet.hpp>
#include <memory>

#include "ORM/User.hpp"

#include "threadlocal.hpp"
#include "util.hpp"

namespace ORM {

std::string const User::SQL_FETCH_BY_ID = "select username, password, display_name from users where id = ?";
std::string const User::SQL_FETCH_BY_USERNAME = "select id, password, display_name from users where username = ?";
std::string const User::SQL_FETCH_BY_DISPLAY_NAME = "select id, username, password from users where display_name = ?";
std::string const User::SQL_UPDATE_DISPLAY_NAME_PASSWORD = "update users set display_name = ?, password = ? where id = ?";
std::string const User::SQL_UPDATE_DISPLAY_NAME = "update users set display_name = ? where id = ?";
std::string const User::SQL_UPDATE_PASSWORD = "update users set password = ? where id = ?";
std::string const User::SQL_DELETE_BY_ID = "delete from users where id = ?";

User::User() {
	// zero-initialize/default-construct all members
}

User::User(id_t id, sql::SQLString username, sql::SQLString password, sql::SQLString display_name)
	: m_id(std::move(id)), m_username(std::move(username)), m_password(std::move(password)), m_display_name(std::move(display_name)) {
	assert(m_password.size() == 64);
}

User::~User() {
	save();
}

void User::save() {
	if (password_dirty && display_name_dirty) {
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_UPDATE_DISPLAY_NAME_PASSWORD) };
		stmt->setString(1, m_display_name);
		stmt->setString(2, m_password);
		stmt->setUInt64(3, m_id);
		stmt->executeQuery();
	} else if (password_dirty && !display_name_dirty) {
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_UPDATE_PASSWORD) };
		stmt->setString(1, m_password);
		stmt->setUInt64(2, m_id);
		stmt->executeQuery();
	} else if (display_name_dirty && !password_dirty) {
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_UPDATE_DISPLAY_NAME) };
		stmt->setString(1, m_display_name);
		stmt->setUInt64(2, m_id);
		stmt->executeQuery();
	}
}

std::optional<User> User::get_by_id(id_t const id) {
	std::unique_ptr<sql::ResultSet> results;
	{
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_FETCH_BY_ID) };
		stmt->setInt(1, id);
		results.reset(stmt->executeQuery());
	}
	if (results->next()) {
		return User{
			id,
			results->getString("username"),
			results->getString("password"),
			results->getString("display_name"),
		};
	} else {
		return std::nullopt;
	}
}
std::optional<User> User::get_by_name(sql::SQLString const& username) {
	std::unique_ptr<sql::ResultSet> results;
	{
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_FETCH_BY_USERNAME) };
		stmt->setString(1, username);
		results.reset(stmt->executeQuery());
	}
	if (results->next()) {
		return User{
			results->getUInt64("id"),
			username,
			results->getString("password"),
			results->getString("display_name"),
		};
	} else {
		return std::nullopt;
	}
}
std::vector<User> User::get_by_display_name(sql::SQLString const& display_name) {
	std::unique_ptr<sql::ResultSet> results;
	{
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_FETCH_BY_DISPLAY_NAME) };
		stmt->setString(1, display_name);
		results.reset(stmt->executeQuery());
	}
	std::vector<User> ret{ results->rowsCount() };
	while (results->next()) {
		ret.emplace_back(results->getUInt64("id"), results->getString("username"), results->getString("password"), display_name);
	}
	return ret;
}
bool User::delete_by_id(id_t const id) {
	std::unique_ptr<sql::ResultSet> results;
	{
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(SQL_DELETE_BY_ID) };
		stmt->setUInt64(1, id);
		stmt->execute();
	}
	{
		std::unique_ptr<sql::PreparedStatement> stmt{ ThreadLocal::conn->prepareStatement(Strings::SQL_GET_ROW_COUNT) };
		results.reset(stmt->executeQuery());
	}
	bool const has_at_least_one_row = results->next();
	assert(has_at_least_one_row);
	// row_count() returns unsigned long long, but there is no appropriate accessor function, so use uint64_t
	auto const deleted_rows = results->getUInt64(1);
	// effectively asserts that id is unique
	assert(deleted_rows == 0 || deleted_rows == 1);
	// assert that it doesn't have more than one row
	assert(!results->next());
	// effectively returns whether the row was found and and deleted
	return deleted_rows == 1;
}
}  // namespace ORM
