#pragma once

#include <string_view>
#include <tao/pq/binary.hpp>

namespace Hash {

struct HashError : public std::exception {
	HashError() {}
	char const* what() const noexcept override;
};
struct HashCreationError : public HashError {
	HashCreationError() {}
	char const* what() const noexcept override;
};
struct SaltError : public HashError {
	SaltError() {}
	char const* what() const noexcept override;
};
struct HashCheckError : public HashError {
	HashCheckError() {}
	char const* what() const noexcept override;
};

tao::pq::binary hash(std::string_view plain, int workload = 12);
bool check(std::string_view plain, tao::pq::binary_view hash);

}  // namespace Hash
