/****************************INFORMATION***********************************
* Copyright (c) 2022 Zontec
* Email: dehibeo@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
**************************************************************************/
/*!
*   @file sha256.h
*   @brief File contains SHA256 API functions.
*	@author Zontec
*	@version 0.1.0
*	@date 2022.06.19
*/

#ifndef SHA256_H
#define SHA256_H

#include "mbcrypt.h"

#define MBCRYPT_SHA256_HASH_SIZE              (32)
#define MBCRYPT_SHA256_BUFFER_SIZE            (64)

typedef struct mbcrypt_sha256_t
{
    uint8_t buffer[MBCRYPT_SHA256_BUFFER_SIZE];
    uint32_t total[2];
    uint32_t h0;
    uint32_t h1;
    uint32_t h2;
    uint32_t h3;
    uint32_t h4;
    uint32_t h5;
    uint32_t h6;
    uint32_t h7;
} mbcrypt_sha256_t;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Function inits sha256 context with default values
 * 
 * @param[in,out] ctx SHA context to be initialized
 * @return mbcrypt_status_e 
 */
mbcrypt_status_e MBCRYPT_API mbcrypt_sha256_init(mbcrypt_sha256_t *ctx);

/**
 * @brief Function updates the context with new hash computations based on 
 * input data.
 * 
 * @param[in,out] ctx SHA256 context
 * @param[in] data Input data to be hashed
 * @param[in] data_len Input data len
 * @return mbcrypt_status_e 
 */
mbcrypt_status_e MBCRYPT_API mbcrypt_sha256_update(mbcrypt_sha256_t *ctx, 
                                            const uint8_t *data, uint32_t data_len);

/**
 * @brief Function finishes computations and produce final hash
 * 
 * @param[in] ctx SHA256 context
 * @param[out] out Output hash. Should be at least allocated SHA256_HASH_SIZE memory
 * @return mbcrypt_status_e 
 */
mbcrypt_status_e MBCRYPT_API mbcrypt_sha256_final(mbcrypt_sha256_t *ctx, uint8_t *out);

/**
 * @brief Function implements step be step three functions: 
 * sha256_init, sha256_update, sha256_finish
 * 
 * @param[in] data Input data to be hashed
 * @param[in] data_len Input data len
 * @param[out] out Output hash. Should be at least allocated SHA256_HASH_SIZE memory
 * @return mbcrypt_status_e 
 */
mbcrypt_status_e MBCRYPT_API mbcrypt_sha256(const uint8_t *data, 
                                            uint32_t data_len, uint8_t *out);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SHA256_H */
