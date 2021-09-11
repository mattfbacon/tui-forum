#pragma once

#include <cstring>
#include <stdexcept>

namespace ORM {

struct ConstraintException : public std::exception {
	ConstraintException(char const* object_name, char const* field_name) : object_name(object_name), field_name(field_name) {}
	char const* what() const noexcept override;
	char const* object_name;
	char const* field_name;
	bool matches(char const* predicate_object_name, char const* predicate_field_name) const noexcept {
		return strcmp(object_name, predicate_object_name) == 0 && strcmp(field_name, predicate_field_name) == 0;
	}
};

}  // namespace ORM
