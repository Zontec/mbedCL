#include "hash_test_vectors.h"
#include "sha256.h"


#if defined(SHA256_UT_GTEST)

#include <iostream>
#include <cstring>
#include "gtest/gtest.h"

/*
*/
TEST(SHA256, FullBasicTest) 
{
    hash_test_vector_t *in;
    hash_test_vector_res_t *res;
    unsigned char out[MBCRYPT_SHA256_HASH_SIZE];

    for (uint32_t i = 0; i < get_number_of_hash_tests(); ++i)
    {
        std::cout << "Test group " << i << " started!" << "\n"; 

        get_test_vector(SHA256_TEST_GROUP, i, &in, &res);
        if (in == NULL || res == NULL)
        {
            std::cerr << "get_test_vector error. Possible index out of range! \n";
            ASSERT_EQ(1, 0);
        }

        ASSERT_EQ(mbcrypt_sha256(in->data_in, in->data_len, out), MBCRYPT_STATUS_OK);
        ASSERT_EQ(std::memcmp(res->res, out, MBCRYPT_SHA256_HASH_SIZE), 0);
        
        std::cout << "Test group " << i << " - OK" << "\n"; 
    }
}




#elif defined(SHA256_UT_CMOCKA)

#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <string.h>
#include "cmocka.h"
void sha256_basic_test(void **state)
{
    hash_test_vector_t *in;
    hash_test_vector_res_t *res;
    unsigned char out[MBCRYPT_SHA256_HASH_SIZE];

    for (uint32_t i = 0; i < get_number_of_hash_tests(); ++i)
    {
        printf("Test group %d started!\n", i);
        get_test_vector(SHA256_TEST_GROUP, i, &in, &res);
        if (in == NULL || res == NULL)
        {
            assert_int_equal(1, 0);
        }

        assert_int_equal(mbcrypt_sha256(in->data_in, in->data_len, out), MBCRYPT_STATUS_OK);
        assert_int_equal(memcmp(res->res, out, MBCRYPT_SHA256_HASH_SIZE), 0);
        printf("Test group %d - OK!\n");

    }
}


#elif defined(SHA256_UT_CUSTOM)


#else
  #error File sha256_ut.c included to the build but test option not selected
#endif



