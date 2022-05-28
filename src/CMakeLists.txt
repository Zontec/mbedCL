

list(APPEND MBCRYPT_SOURCES mbcrypt_utils.c)

if(${MBCRYPT_SHA1_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES sha1.c)
endif()

if(${MBCRYPT_SHA256_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES sha256.c)
endif()

if(${MBCRYPT_SHA512_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES sha512.c)
endif()

if(${MBCRYPT_HMAC_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES hmac.c)
endif()

if(${MBCRYPT_PBKDF2_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES pbkdf2.c)
endif()

if(${MBCRYPT_KBKDF_ENABLED} STREQUAL ON)
    list(APPEND MBCRYPT_SOURCES kbkdf.c)
endif()


add_library (mbcrypt_src ${MBCRYPT_SOURCES})

target_include_directories(mbcrypt_src PUBLIC ../includes/)
