#include <array>
#include <bcrypt/bcrypt.h>

#include "Hash.hpp"

namespace Hash {

char const* HashError::what() const noexcept {
	return "HashError";
}
char const* HashCreationError::what() const noexcept {
	return "HashCreationError";
}
char const* SaltError::what() const noexcept {
	return "SaltError";
}
char const* HashCheckError::what() const noexcept {
	return "HashCheckError";
}

tao::pq::binary hash(std::string_view const plain, int const workload) {
	std::array<std::byte, BCRYPT_HASHSIZE> salt;
	tao::pq::binary hash;
	hash.resize(BCRYPT_HASHSIZE);

	if (bcrypt_gensalt(workload, (char*)salt.data()) != 0) {
		throw SaltError{};
	}
	if (bcrypt_hashpw(plain.data(), (char*)salt.data(), (char*)hash.data()) != 0) {
		throw HashCreationError{};
	}
	return hash;
}

bool check(std::string_view const plain, tao::pq::binary_view const hash) {
	auto const ret = bcrypt_checkpw(plain.data(), (char*)hash.data());
	if (ret == -1) {
		throw HashCheckError{};
	}
	return ret == 0;
}

}  // namespace Hash
