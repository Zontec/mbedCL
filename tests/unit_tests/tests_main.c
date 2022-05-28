#if defined(SHA256_UT_GTEST)

#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#else
#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <string.h>
#include "cmocka.h"

extern void sha256_basic_test(void **state);
 
int main(void) {
const struct CMUnitTest tests[] = {
cmocka_unit_test(sha256_basic_test),
};
return cmocka_run_group_tests(tests, NULL, NULL);
}

#endif