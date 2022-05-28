cmake_minimum_required(VERSION 3.10.0)

project(mbedcrypto)

set(LIBTOMMATH_PATH lib/libtommath)
set( CMAKE_CXX_COMPILER "g++" )
set( CMAKE_C_COMPILER "gcc" )

option(BUILD_SHARED "Build shared library" ON)
option(BUILD_STATIC "Build static library" ON)
option(BUILD_TEST_RUNNER "Build selftest runner" ON)
option(ONE_FILE_API "Build API to one file" ON)
option(CAVP_TEST_INSTALL "Install libraries for CAVP testing" ON)
option(INSTALL_LIBRARY "Install libraries for unit testing and usage" ON)

########### @SHA1 OPTIONS ###########
option(MBCRYPT_SHA1_ENABLED "SHA1 build" ON)
option(MBCRYPT_SHA1_SELFTEST "Selftest for SHA1" ON)
set(MBCRYPT_SHA1_SECURITY_LEVEL 0 CACHE STRING "SHA1 security level")

########### @SHA256 OPTIONS ###########
option(MBCRYPT_SHA256_ENABLED "SHA256 build" ON)
option(MBCRYPT_SHA256_SELFTEST "Selftest for SHA256" ON)
set(MBCRYPT_SHA256_SECURITY_LEVEL 0 CACHE STRING "SHA256 security level")

########### @SHA512 OPTIONS ###########
option(MBCRYPT_SHA512_ENABLED "SHA512 build" ON)
option(MBCRYPT_SHA512_SELFTEST "Selftest for SHA512" ON)
set(MBCRYPT_SHA512_SECURITY_LEVEL 0 CACHE STRING "SHA512 security level")

########### @HMAC OPTIONS ###########
option(MBCRYPT_HMAC_ENABLED "HMAC build" ON)
option(MBCRYPT_HMAC_SELFTEST "Selftest for HMAC" ON)
set(MBCRYPT_HMAC_SECURITY_LEVEL 0 CACHE STRING "HMAC security level")

########### @PBKDF2 OPTIONS ###########
option(MBCRYPT_PBKDF2_ENABLED "PBKDF2 build" ON)
option(MBCRYPT_PBKDF2_SELFTEST "Selftest for PBKDF2" ON)
set(MBCRYPT_PBKDF2_SECURITY_LEVEL 0 CACHE STRING "PBKDF2 security level")

########### @PBKDF2 OPTIONS ###########
option(MBCRYPT_KBKDF_ENABLED "KBKDF build" ON)
option(MBCRYPT_KBKDF_SELFTEST "Selftest for KBKDF" ON)
set(MBCRYPT_KBKDF_SECURITY_LEVEL 0 CACHE STRING "KBKDF security level")

########### @RSA OPTIONS ###########
option(MBCRYPT_RSA_ENABLED "RSA build" ON)
option(MBCRYPT_RSA_SELFTEST "Selftest for RSA" ON)
set(MBCRYPT_RSA_SECURITY_LEVEL 0 CACHE STRING "RSA security level")

########### @AES OPTIONS ###########
option(MBCRYPT_AES_ENABLED "AES build" ON)
option(MBCRYPT_AES_SELFTEST "Selftest for AES" ON)
set(MBCRYPT_AES_SECURITY_LEVEL 0 CACHE STRING "AES security level")

########### @RC4 OPTIONS ###########
option(MBCRYPT_RC4_ENABLED "RC4 build" ON)
option(MBCRYPT_RC4_SELFTEST "Selftest for RC4" ON)
set(MBCRYPT_RC4_SECURITY_LEVEL 0 CACHE STRING "RC4 security level")

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(arch)

add_library (myshared SHARED src/self_tests.c)
add_library (mystatic STATIC src/self_tests.c)
add_executable (mbedcrypto src/self_tests.c)
target_link_libraries(mbedcrypto mbcrypt_src libtommath)
target_link_libraries(myshared mbcrypt_src libtommath)
target_link_libraries(mystatic mbcrypt_src libtommath)

target_include_directories(mbedcrypto PUBLIC ./includes/)
target_include_directories(myshared PUBLIC ./includes/)
target_include_directories(mystatic PUBLIC ./includes/)
