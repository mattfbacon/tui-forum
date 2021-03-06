set(bcrypt_sources bcrypt.c crypt_blowfish.c crypt_gensalt.c wrapper.c x86.S)
list(TRANSFORM bcrypt_sources PREPEND ${CMAKE_CURRENT_LIST_DIR}/bcrypt/src/)
add_library(bcrypt STATIC ${bcrypt_sources})
target_include_directories(bcrypt PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bcrypt/include/bcrypt)
target_include_directories(bcrypt SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/bcrypt/include)
