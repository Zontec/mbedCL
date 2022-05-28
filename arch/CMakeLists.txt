if (PLATFORM STQL "arm")

list(arm/APPEND MBCRYPT_SOURCES armv7_init.c)
list(arm/APPEND MBCRYPT_SOURCES armv7_isr.c)
list(arm/APPEND MBCRYPT_SOURCES armv7_kernel_test.c)
list(arm/APPEND MBCRYPT_SOURCES armv7_mem_test.c)
list(arm/APPEND MBCRYPT_SOURCES armv7_platform_specific.c)

end()
add_library (mbcrypt_src ${MBCRYPT_SOURCES})

target_include_directories(mbcrypt_src PUBLIC ../includes/)
