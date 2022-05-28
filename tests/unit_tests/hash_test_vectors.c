

#include "hash_test_vectors.h"

#define U_STR (unsigned const char *)


static hash_test_vector_t test_vectors[] = 
{
    /*===================== TEST VECTOR 1 =====================*/
    {
        /* data_in */
        U_STR"\x00",
        /* data_len */
        1,
    },
    /*===================== TEST VECTOR 2 =====================*/
    {
        /* data_in */
        U_STR"Hello world!",
        /* data_len */
        12,
    },
    /*===================== TEST VECTOR 3 =====================*/
    {
        /* data_in */
        U_STR"mbcrypt:)",
        /* data_len */
        9,
    },
};


#define HASH_TESTS (uint32_t)(sizeof(test_vectors)/sizeof(test_vectors[0]))


#if defined(SHA256_UT)

#include "sha256.h"


static hash_test_vector_res_t sha256_exp_res[]= 
{
    /*===================== TEST VECTOR RES 1 =====================*/
    {U_STR"\x6E\x34\x0B\x9C\xFF\xB3\x7A\x98\x9C\xA5\x44\xE6\xBB\x78\x0A\x2C\x78\x90\x1D\x3F\xB3\x37\x38\x76\x85\x11\xA3\x06\x17\xAF\xA0\x1D"},
    /*===================== TEST VECTOR RES 2 =====================*/
    {U_STR"\xC0\x53\x5E\x4B\xE2\xB7\x9F\xFD\x93\x29\x13\x05\x43\x6B\xF8\x89\x31\x4E\x4A\x3F\xAE\xC0\x5E\xCF\xFC\xBB\x7D\xF3\x1A\xD9\xE5\x1A"},
    /*===================== TEST VECTOR RES 3 =====================*/
    {U_STR"\xE5\x75\x10\x0C\x87\xB1\x44\xCA\xE6\x16\x51\x9A\x82\xB0\x4D\x4C\xF4\xBE\x97\x56\xF9\xFB\x6A\x51\x26\x03\x0F\xD5\x07\xE3\xDF\x63"},
};


void get_test_vector_sha256(uint32_t vector_index,  
                             hash_test_vector_t ** data,  hash_test_vector_res_t ** res)
{
    *data = &test_vectors[vector_index];
    *res = &sha256_exp_res[vector_index];
}

//static_assert(HASH_TESTS == (uint32_t)(sizeof(sha256_exp_res)/sizeof(sha256_exp_res[0])), 
//                                "Number of test vectors don't match with number or resultes for SHA256 ut");


#endif /* SHA256_UT */


uint32_t get_number_of_hash_tests()
{
    return HASH_TESTS;
}

void get_test_vector(test_hash_vector_group_e test_group, uint32_t vector_index,  
                         hash_test_vector_t ** data,  hash_test_vector_res_t **res)
{
    switch (test_group)
    {
    case SHA256_TEST_GROUP:
        get_test_vector_sha256(vector_index, data, res);
        break;
    default:
        data = NULL;
        res = NULL;
        break;
    }
}
