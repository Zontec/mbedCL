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
*   @file sha256.c
*   @brief File contains SHA-256 implementation.
*	@author Zontec
*	@version 0.1.0
*	@date 2022.07.04
*/

#include "sha256.h"

#define SHIFT_RIGHT(x, n)   ((x & MAX_UINT32) >> n)
#define ROT_RIGHT(x, n)     (SHIFT_RIGHT(x, n) | (x << (32 - n)))

#define SUB0(x)             (ROT_RIGHT(x, 7) ^ ROT_RIGHT(x, 18) ^  SHIFT_RIGHT(x, 3))
#define SUB1(x)             (ROT_RIGHT(x, 17) ^ ROT_RIGHT(x, 19) ^  SHIFT_RIGHT(x, 10))
#define SUB2(x)             (ROT_RIGHT(x, 2) ^ ROT_RIGHT(x, 13) ^ ROT_RIGHT(x, 22))
#define SUB3(x)             (ROT_RIGHT(x, 6) ^ ROT_RIGHT(x, 11) ^ ROT_RIGHT(x, 25))

#define F0(x, y, z)         ((x & y) | (z & (x | y)))
#define F1(x, y, z)         (z ^ (x & (y ^ z)))

#define ROT(t)                                  \
(                                               \
    W[t] = SUB1(W[t - 2]) + W[t - 7] +          \
           SUB0(W[t - 15]) + W[t - 16]          \
)

#define PAD(a, b, c, d, e, f, g, h, x, K_TMP) do    \
{                                                   \
    tmp1 = h + SUB3(e) + F1(e, f, g) + K_TMP + x;   \
    tmp2 = SUB2(a) + F0(a, b, c);                   \
    d += tmp1;                                      \
    h = tmp1 + tmp2;                                \
} while (0)


static const uint8_t sha256_padding[MBCRYPT_SHA256_BUFFER_SIZE] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const uint32_t K_TMP[MBCRYPT_SHA256_BUFFER_SIZE] =
{
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};


static void sha256_process(mbcrypt_sha256_t *ctx, const uint8_t *data)
{
    uint32_t tmp1;
    uint32_t tmp2;
    uint32_t W[MBCRYPT_SHA256_BUFFER_SIZE];
    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
    uint32_t E;
    uint32_t F;
    uint32_t G;
    uint32_t H;

    A = ctx->h0;
    B = ctx->h1;
    C = ctx->h2;
    D = ctx->h3;
    E = ctx->h4;
    F = ctx->h5;
    G = ctx->h6;
    H = ctx->h7;

/* MBCRYPT_SHA256_MIN_SIZE not unfoldes the for loop  */
#if defined(MBCRYPT_SHA256_MIN_SIZE)

    for (uint32_t i = 0; i < 16; ++i)
    {
        GET_UINT32_BE(W[i], data, (i << 2));
    }

    for (uint32_t i = 0; i < 16; i+= 8)
    {
        PAD(A, B, C, D, E, F, G, H, W[i + 0], K_TMP[i + 0]);
        PAD(H, A, B, C, D, E, F, G, W[i + 1], K_TMP[i + 1]);
        PAD(G, H, A, B, C, D, E, F, W[i + 2], K_TMP[i + 2]);
        PAD(F, G, H, A, B, C, D, E, W[i + 3], K_TMP[i + 3]);
        PAD(E, F, G, H, A, B, C, D, W[i + 4], K_TMP[i + 4]);
        PAD(D, E, F, G, H, A, B, C, W[i + 5], K_TMP[i + 5]);
        PAD(C, D, E, F, G, H, A, B, W[i + 6], K_TMP[i + 6]);
        PAD(B, C, D, E, F, G, H, A, W[i + 7], K_TMP[i + 7]);
    }

    for (uint32_t i = 16; i < 64; i += 8)
    {
        PAD(A, B, C, D, E, F, G, H, ROT(i + 0), K_TMP[i + 0]);
        PAD(H, A, B, C, D, E, F, G, ROT(i + 1), K_TMP[i + 1]);
        PAD(G, H, A, B, C, D, E, F, ROT(i + 2), K_TMP[i + 2]);
        PAD(F, G, H, A, B, C, D, E, ROT(i + 3), K_TMP[i + 3]);
        PAD(E, F, G, H, A, B, C, D, ROT(i + 4), K_TMP[i + 4]);
        PAD(D, E, F, G, H, A, B, C, ROT(i + 5), K_TMP[i + 5]);
        PAD(C, D, E, F, G, H, A, B, ROT(i + 6), K_TMP[i + 6]);
        PAD(B, C, D, E, F, G, H, A, ROT(i + 7), K_TMP[i + 7]);
    }

#else /* MBCRYPT_SHA256_MIN_SIZE */

    GET_UINT32_BE(W[0], data, 0);
    GET_UINT32_BE(W[1], data, 4);
    GET_UINT32_BE(W[2], data, 8);
    GET_UINT32_BE(W[3], data, 12);
    GET_UINT32_BE(W[4], data, 16);
    GET_UINT32_BE(W[5], data, 20);
    GET_UINT32_BE(W[6], data, 24);
    GET_UINT32_BE(W[7], data, 28);
    GET_UINT32_BE(W[8], data, 32);
    GET_UINT32_BE(W[9], data, 36);
    GET_UINT32_BE(W[10], data, 40);
    GET_UINT32_BE(W[11], data, 44);
    GET_UINT32_BE(W[12], data, 48);
    GET_UINT32_BE(W[13], data, 52);
    GET_UINT32_BE(W[14], data, 56);
    GET_UINT32_BE(W[15], data, 60);
    
    PAD(A, B, C, D, E, F, G, H, W[0], K_TMP[0]);
    PAD(H, A, B, C, D, E, F, G, W[1], K_TMP[1]);
    PAD(G, H, A, B, C, D, E, F, W[2], K_TMP[2]);
    PAD(F, G, H, A, B, C, D, E, W[3], K_TMP[3]);
    PAD(E, F, G, H, A, B, C, D, W[4], K_TMP[4]);
    PAD(D, E, F, G, H, A, B, C, W[5], K_TMP[5]);
    PAD(C, D, E, F, G, H, A, B, W[6], K_TMP[6]);
    PAD(B, C, D, E, F, G, H, A, W[7], K_TMP[7]);

    PAD(A, B, C, D, E, F, G, H, W[8], K_TMP[8]);
    PAD(H, A, B, C, D, E, F, G, W[9], K_TMP[9]);
    PAD(G, H, A, B, C, D, E, F, W[10], K_TMP[10]);
    PAD(F, G, H, A, B, C, D, E, W[11], K_TMP[11]);
    PAD(E, F, G, H, A, B, C, D, W[12], K_TMP[12]);
    PAD(D, E, F, G, H, A, B, C, W[13], K_TMP[13]);
    PAD(C, D, E, F, G, H, A, B, W[14], K_TMP[14]);
    PAD(B, C, D, E, F, G, H, A, W[15], K_TMP[15]);

    PAD(A, B, C, D, E, F, G, H, ROT(16), K_TMP[16]);
    PAD(H, A, B, C, D, E, F, G, ROT(17), K_TMP[17]);
    PAD(G, H, A, B, C, D, E, F, ROT(18), K_TMP[18]);
    PAD(F, G, H, A, B, C, D, E, ROT(19), K_TMP[19]);
    PAD(E, F, G, H, A, B, C, D, ROT(20), K_TMP[20]);
    PAD(D, E, F, G, H, A, B, C, ROT(21), K_TMP[21]);
    PAD(C, D, E, F, G, H, A, B, ROT(22), K_TMP[22]);
    PAD(B, C, D, E, F, G, H, A, ROT(23), K_TMP[23]);

    PAD(A, B, C, D, E, F, G, H, ROT(24), K_TMP[24]);
    PAD(H, A, B, C, D, E, F, G, ROT(25), K_TMP[25]);
    PAD(G, H, A, B, C, D, E, F, ROT(26), K_TMP[26]);
    PAD(F, G, H, A, B, C, D, E, ROT(27), K_TMP[27]);
    PAD(E, F, G, H, A, B, C, D, ROT(28), K_TMP[28]);
    PAD(D, E, F, G, H, A, B, C, ROT(29), K_TMP[29]);
    PAD(C, D, E, F, G, H, A, B, ROT(30), K_TMP[30]);
    PAD(B, C, D, E, F, G, H, A, ROT(31), K_TMP[31]);

    PAD(A, B, C, D, E, F, G, H, ROT(32), K_TMP[32]);
    PAD(H, A, B, C, D, E, F, G, ROT(33), K_TMP[33]);
    PAD(G, H, A, B, C, D, E, F, ROT(34), K_TMP[34]);
    PAD(F, G, H, A, B, C, D, E, ROT(35), K_TMP[35]);
    PAD(E, F, G, H, A, B, C, D, ROT(36), K_TMP[36]);
    PAD(D, E, F, G, H, A, B, C, ROT(37), K_TMP[37]);
    PAD(C, D, E, F, G, H, A, B, ROT(38), K_TMP[38]);
    PAD(B, C, D, E, F, G, H, A, ROT(39), K_TMP[39]);
    
    PAD(A, B, C, D, E, F, G, H, ROT(40), K_TMP[40]);
    PAD(H, A, B, C, D, E, F, G, ROT(41), K_TMP[41]);
    PAD(G, H, A, B, C, D, E, F, ROT(42), K_TMP[42]);
    PAD(F, G, H, A, B, C, D, E, ROT(43), K_TMP[43]);
    PAD(E, F, G, H, A, B, C, D, ROT(44), K_TMP[44]);
    PAD(D, E, F, G, H, A, B, C, ROT(45), K_TMP[45]);
    PAD(C, D, E, F, G, H, A, B, ROT(46), K_TMP[46]);
    PAD(B, C, D, E, F, G, H, A, ROT(47), K_TMP[47]);

    PAD(A, B, C, D, E, F, G, H, ROT(48), K_TMP[48]);
    PAD(H, A, B, C, D, E, F, G, ROT(49), K_TMP[49]);
    PAD(G, H, A, B, C, D, E, F, ROT(50), K_TMP[50]);
    PAD(F, G, H, A, B, C, D, E, ROT(51), K_TMP[51]);
    PAD(E, F, G, H, A, B, C, D, ROT(52), K_TMP[52]);
    PAD(D, E, F, G, H, A, B, C, ROT(53), K_TMP[53]);
    PAD(C, D, E, F, G, H, A, B, ROT(54), K_TMP[54]);
    PAD(B, C, D, E, F, G, H, A, ROT(55), K_TMP[55]);

    PAD(A, B, C, D, E, F, G, H, ROT(56), K_TMP[56]);
    PAD(H, A, B, C, D, E, F, G, ROT(57), K_TMP[57]);
    PAD(G, H, A, B, C, D, E, F, ROT(58), K_TMP[58]);
    PAD(F, G, H, A, B, C, D, E, ROT(59), K_TMP[59]);
    PAD(E, F, G, H, A, B, C, D, ROT(60), K_TMP[60]);
    PAD(D, E, F, G, H, A, B, C, ROT(61), K_TMP[61]);
    PAD(C, D, E, F, G, H, A, B, ROT(62), K_TMP[62]);
    PAD(B, C, D, E, F, G, H, A, ROT(63), K_TMP[63]);

#endif /* MBCRYPT_SHA256_MIN_SIZE */

    ctx->h0 += A;
    ctx->h1 += B;
    ctx->h2 += C;
    ctx->h3 += D;
    ctx->h4 += E;
    ctx->h5 += F;
    ctx->h6 += G;
    ctx->h7 += H;

#if (MBCRYPT_SECURITY_LEVEL == MBCRYPT_SECURITY_LEVEL_2) || defined(MBCRYPT_SECURED_SHA256)
    tmp1 = MAX_UINT32;
    tmp2 = MAX_UINT32;
    A = MAX_UINT32;
    B = MAX_UINT32;
    C = MAX_UINT32;
    D = MAX_UINT32;
    E = MAX_UINT32;
    F = MAX_UINT32;
    G = MAX_UINT32;
    H = MAX_UINT32;
    /* MBCRYPT_SHA256_BUFFER_SIZE * sizeof(uint32_t) */
    memset_safe(W, MAX_BYTE_VALUE, 256);
#endif /* MBCRYPT_SECURED_SHA256 */
}


mbcrypt_status_e mbcrypt_sha256_init(mbcrypt_sha256_t *ctx)
{
MBCRYPT_FUNCTION_BEGIN;

    MBCRYPT_CHECK_VALID_NOT_NULL(ctx);

    MBCRYPT_CHECK_VALID_NOT_NULL(memset(ctx, 0x00, sizeof(mbcrypt_sha256_t)));

    ctx->h0 = 0x6A09E667;
    ctx->h1 = 0xBB67AE85;
    ctx->h2 = 0x3C6EF372;
    ctx->h3 = 0xA54FF53A;
    ctx->h4 = 0x510E527F;
    ctx->h5 = 0x9B05688C;
    ctx->h6 = 0x1F83D9AB;
    ctx->h7 = 0x5BE0CD19;

MBCRYPT_FUNCTION_EXIT:
    MBCRYPT_FUNCTION_RETURN;
}


mbcrypt_status_e mbcrypt_sha256_update(mbcrypt_sha256_t *ctx, 
                                        const uint8_t *data, uint32_t data_len)
{
MBCRYPT_FUNCTION_BEGIN;

    uint32_t fill;
    uint32_t left;

    MBCRYPT_CHECK_VALID_NOT_NULL(data);
    MBCRYPT_CHECK_VALID_NOT_NULL(ctx);

    if (data_len == 0)
    {
        goto MBCRYPT_FUNCTION_EXIT;
    }

    left = ctx->total[0] & (uint32_t)(0b00111111); // 63 == 0x3F
    fill = MBCRYPT_SHA256_BUFFER_SIZE - left;

    ctx->total[0] += data_len;
    ctx->total[0] &= MAX_UINT32;

    if (ctx->total[0] < data_len)
    {
        ++ctx->total[1];
    }

    if (left && data_len >= fill)
    {
        MBCRYPT_CHECK_VALID_NOT_NULL(memcpy((void *)(ctx->buffer + left), 
                                            data, fill));
        sha256_process(ctx, ctx->buffer);
        data += fill;
        data_len -= fill;
        left = 0;
    }

    /* if not a complite package */
    while (data_len >= MBCRYPT_SHA256_BUFFER_SIZE)
    {
        sha256_process(ctx, data);
        data += MBCRYPT_SHA256_BUFFER_SIZE;
        data_len -= MBCRYPT_SHA256_BUFFER_SIZE;
    }

    if (data_len > 0)
    {
        MBCRYPT_CHECK_VALID_NOT_NULL(memcpy((void *)(ctx->buffer + left), 
                                        data, data_len));
    }


MBCRYPT_FUNCTION_EXIT:

#if (MBCRYPT_SECURITY_LEVEL == MBCRYPT_SECURITY_LEVEL_2) || defined(MBCRYPT_SECURED_SHA256)
    fill = MAX_UINT32;
    left = MAX_UINT32;
#endif /* MBCRYPT_SECURED_SHA256 */

    MBCRYPT_FUNCTION_RETURN;
}


mbcrypt_status_e mbcrypt_sha256_final(mbcrypt_sha256_t *ctx, uint8_t *out)
{
MBCRYPT_FUNCTION_BEGIN;

    uint32_t last;
    uint32_t padn;
    uint32_t high;
    uint32_t low;
    uint8_t msglen[8];

    MBCRYPT_CHECK_VALID_NOT_NULL(ctx);
    MBCRYPT_CHECK_VALID_NOT_NULL(out);

    /* message len in BE */
    high = (ctx->total[0] >> 29)
         | (ctx->total[1] << 3);
    low  = (ctx->total[0] << 3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low, msglen, 4);

    last = ctx->total[0] & (uint32_t)(0x3F);
    padn = (last < 56) ? (56 - last) : (120 - last);

    MBCRYPT_CHECK_RES(mbcrypt_sha256_update(ctx, sha256_padding, padn));
    MBCRYPT_CHECK_RES(mbcrypt_sha256_update(ctx, msglen, 8));

    PUT_UINT32_BE(ctx->h0, out, 0);
    PUT_UINT32_BE(ctx->h1, out, 4);
    PUT_UINT32_BE(ctx->h2, out, 8);
    PUT_UINT32_BE(ctx->h3, out, 12);
    PUT_UINT32_BE(ctx->h4, out, 16);
    PUT_UINT32_BE(ctx->h5, out, 20);
    PUT_UINT32_BE(ctx->h6, out, 24);
    PUT_UINT32_BE(ctx->h7, out, 28);


MBCRYPT_FUNCTION_EXIT:

#if (MBCRYPT_SECURITY_LEVEL == MBCRYPT_SECURITY_LEVEL_2) || defined(MBCRYPT_SECURED_SHA256)
    last = MAX_UINT32;
    padn = MAX_UINT32;
    high = MAX_UINT32;
    low =  MAX_UINT32;
    memset_safe(msglen, MAX_BYTE_VALUE, sizeof(msglen));
#endif /* MBCRYPT_SECURED_SHA256 */

    MBCRYPT_FUNCTION_RETURN;
}


mbcrypt_status_e mbcrypt_sha256(const uint8_t *data, uint32_t data_len, uint8_t *out)
{
MBCRYPT_FUNCTION_BEGIN;

    mbcrypt_sha256_t ctx;

    MBCRYPT_CHECK_RES(mbcrypt_sha256_init(&ctx));
    MBCRYPT_CHECK_RES(mbcrypt_sha256_update(&ctx, data, data_len));
    MBCRYPT_CHECK_RES(mbcrypt_sha256_final(&ctx, out));

MBCRYPT_FUNCTION_EXIT:

#if (MBCRYPT_SECURITY_LEVEL == MBCRYPT_SECURITY_LEVEL_2) || defined(MBCRYPT_SECURED_SHA256)
    memset_safe(&ctx, MAX_BYTE_VALUE, sizeof(ctx));
#endif /* MBCRYPT_SECURED_SHA256 */

    MBCRYPT_FUNCTION_RETURN;
}