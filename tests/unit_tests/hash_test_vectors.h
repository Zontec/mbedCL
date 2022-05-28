#ifndef HASH_TEST_VECTORS_H
#define HASH_TEST_VECTORS_H

#include "mbcrypt.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct hash_test_vector
{
    const uint8_t *data_in;
    uint32_t data_len;
} hash_test_vector_t;

typedef struct hash_test_vector_res
{
    const uint8_t *res;
} hash_test_vector_res_t;

typedef enum test_hash_vector_group
{
    SHA1_TEST_GROUP =   0,
    SHA256_TEST_GROUP = 1,
} test_hash_vector_group_e;


void get_test_vector(test_hash_vector_group_e test_group, uint32_t vector_index,  
                         hash_test_vector_t ** data,  hash_test_vector_res_t **res);

uint32_t get_number_of_hash_tests();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HASH_TEST_VECTORS_H */
