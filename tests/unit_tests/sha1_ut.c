#define SHA1_UT_GTEST

#if defined(SHA1_UT_GTEST)

#include <iostream>
#include "gtest/gtest.h"
#include "sha1.h"

float sqr(float a){
    return a;
}

TEST(SquareRootTest, PositiveNos) { 

        unsigned char out[MBCRYPT_SHA1_HASH_SIZE];

        mbcrypt_sha1((unsigned const char*)"sdfaasdf", 8, out);
        for (auto &i: out){
            printf("%02X", i);
        }
        EXPECT_EQ (18.0, sqr(324.0));
        EXPECT_EQ (25.4, sqr(645.16));
        EXPECT_EQ (50.3321, sqr(2533.310224));
}

TEST (SquareRootTest, ZeroAndNegativeNos) { 
        ASSERT_EQ (0.0, sqr(0.0));
        ASSERT_EQ (-1, sqr(-22.0));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#elif defined(SHA1_UT_CMOCKA)
    #error SHA1_UT_CUSTOM not implemented yet
#elif defined(SHA1_UT_CUSTOM)
    #error SHA1_UT_CUSTOM not implemented yet
#else
    #error File sha256_ut.c included to the build but test option not selected
#endif



