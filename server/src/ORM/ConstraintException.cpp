#include "ORM/ConstraintException.hpp"

namespace ORM {

char const* ConstraintException::what() const noexcept {
	return "ConstraintException";
}

}  // namespace ORM
