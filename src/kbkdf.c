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
*   @file kbkdf.c
*   @brief File contains KBKDF implementation.
*	@author Zontec
*	@version 1.1
*	@date 2022.07.04
*/

#include "kbkdf.h"

#define HMAC_MAX_OUTPUT     (64)
#define BITS_IN_BYTE        (8)
#define RLEN                (4) // RLEN is limited with uint32_t type

#ifndef BIT_MASK_LEFT
static uint8_t _bit_mask_left[] = {
    0,
    0x80,
    0x80 + 0x40,
    0x80 + 0x40 + 0x20,
    0x80 + 0x40 + 0x20 + 0x10,
    0x80 + 0x40 + 0x20 + 0x10 + 0x08,
    0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04,
    0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04 + 0x02,
    0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04 + 0x02 + 0x01,
};
#define BIT_MASK_LEFT(n) (_bit_mask_left[n])
#endif

#ifndef BIT_MASK_RIGHT
static uint8_t _bit_mask_right[] = {
    0,
    0x01,
    0x02 + 0x01,
    0x04 + 0x02 + 0x01,
    0x08 + 0x04 + 0x02 + 0x01,
    0x10 + 0x08 + 0x04 + 0x02 + 0x01,
    0x20 + 0x10 + 0x08 + 0x04 + 0x02 + 0x01,
    0x40 + 0x20 + 0x10 + 0x08 + 0x04 + 0x02 + 0x01,
    0x80 + 0x40 + 0x20 + 0x10 + 0x08 + 0x04 + 0x02 + 0x01,
};
#define BIT_MASK_RIGHT(n) (_bit_mask_right[n])
#endif

static void _shift_array_right(uint8_t *array, uint32_t length, uint32_t shift, uint8_t carry) // 1 <= shift <= 7
{
    uint8_t temp;

    for (uint32_t i = 0; i < length; i++)
    {
        temp = array[i];                        // save the value
        array[i] = (array[i] >> shift) | carry; // update the array element
        carry = temp << (8 - shift);            // compute the new carry
    }
}

static uint32_t _mbcrypt_kbkdf_counter(mbcrypt_hash_type_e hash_type, 
                               mbcrypt_hmac_callbacks_t *cbs,
                               const uint8_t *key_in, const uint32_t key_in_len,
                               uint8_t *fixed_input, const uint32_t fixed_input_len,
                               uint8_t *key_out, const uint32_t key_out_len,
                               mbcrypt_kbkdf_opts_t *opts)
{
MBCRYPT_FUNCTION_BEGIN;

    mbcrypt_hmac_init_t hmac_init = cbs->hmac_init;
    mbcrypt_hmac_update_t hmac_update = cbs->hmac_update;
    mbcrypt_hmac_final_t hmac_final = cbs->hmac_final;
    void *ctx = cbs->hmac_ctx;

    uint32_t hmac_size;
    uint32_t full_blocks;
    uint32_t leftover;
    uint32_t modulo;
    uint32_t rpos;
    uint32_t rpos_fixed;
    uint8_t rpos_modulo;

    uint8_t *key_out_last;
    uint8_t ctr[RLEN + 1];

    if (opts->ctr_rlen > RLEN)
    {
        MBCRYPT_FUNCTION_RET_VAR = MBCRYPT_STATUS_FAIL;
        goto MBCRYPT_FUNCTION_EXIT;
    }

    hmac_size = GET_HASH_SIZE_BY_HASH_TYPE(hash_type);

    rpos_modulo = opts->ctr_rpos % 8;
    rpos = opts->ctr_rpos / 8;
    rpos_fixed = rpos + ((rpos_modulo > 0) ? 1 : 0);

    modulo = ((key_out_len % 8) > 1) ? 1 : 0;
    full_blocks = ((key_out_len / 8) + modulo) / hmac_size;
    leftover = ((key_out_len / 8) + modulo) % hmac_size;
    key_out_last = key_out;

    for (uint32_t i = 1; i <= full_blocks; ++i, key_out += hmac_size)
    {
        PUT_UINT32_BE(i, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));

        if (rpos > 0)
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, rpos));
        }
        if (opts->ctr_rlen > 0)
        {
            if (rpos_modulo > 0)
            {
                _shift_array_right(ctr + (RLEN - opts->ctr_rlen), 
                                    opts->ctr_rlen + 1, rpos_modulo, 
                                    (fixed_input[rpos] & BIT_MASK_LEFT(rpos_modulo)));
                ctr[4] |= fixed_input[rpos] & BIT_MASK_RIGHT(8 - rpos_modulo);
            }
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), 
                                opts->ctr_rlen + ((rpos_modulo > 0) ? 1 : 0)));
        }
        if ((rpos_fixed != fixed_input_len) &&
            (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input + rpos_fixed, 
                                            fixed_input_len - rpos_fixed) != MBCRYPT_STATUS_OK))
        {
            goto MBCRYPT_FUNCTION_EXIT;
        }

        MBCRYPT_CHECK_RES(hmac_final(ctx, key_out));
        key_out_last = key_out;
    }
    if (leftover)
    {
        uint8_t last_block[HMAC_MAX_OUTPUT];

        PUT_UINT32_BE(full_blocks + 1, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));

        if (rpos > 0)
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, rpos));
        }
        if (opts->ctr_rlen > 0)
        {
            if (rpos_modulo > 0)
            {
                _shift_array_right(ctr + (RLEN - opts->ctr_rlen), 
                                    opts->ctr_rlen + 1, rpos_modulo, 
                                    (fixed_input[rpos] & BIT_MASK_LEFT(rpos_modulo)));
                ctr[4] |= fixed_input[rpos] & BIT_MASK_RIGHT(8 - rpos_modulo);
            }
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), 
                            opts->ctr_rlen + ((rpos_modulo > 0) ? 1 : 0)));
        }
        if ((rpos_fixed != fixed_input_len) &&
            (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input + rpos_fixed, 
                                            fixed_input_len - rpos_fixed) != MBCRYPT_STATUS_OK))
        {
            goto MBCRYPT_FUNCTION_EXIT;
        }

        MBCRYPT_CHECK_RES(hmac_final(ctx, last_block));

        MBCRYPT_CHECK_VALID_NOT_NULL(memcpy(key_out, last_block, leftover));
        MBCRYPT_CHECK_VALID_NOT_NULL(memset(last_block, 0xFF, HMAC_MAX_OUTPUT));

        if (modulo)
        {
            key_out[leftover - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }
    else
    {
        if (modulo)
        {
            key_out_last[hmac_size - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }

MBCRYPT_FUNCTION_EXIT:
    MBCRYPT_FUNCTION_RETURN;
}

static uint32_t _mbcrypt_kbkdf_feedback(mbcrypt_hash_type_e hash_type, 
                                mbcrypt_hmac_callbacks_t *cbs,
                                const uint8_t *key_in, const uint32_t key_in_len,
                                const uint8_t *iv_in, const uint32_t iv_in_len,
                                uint8_t *fixed_input, const uint32_t fixed_input_len,
                                uint8_t *key_out, const uint32_t key_out_len,
                                mbcrypt_kbkdf_opts_t *opts)
{
MBCRYPT_FUNCTION_BEGIN;

    mbcrypt_hmac_init_t hmac_init = cbs->hmac_init;
    mbcrypt_hmac_update_t hmac_update = cbs->hmac_update;
    mbcrypt_hmac_final_t hmac_final = cbs->hmac_final;
    void *ctx = cbs->hmac_ctx;

    const uint8_t *k_i_1 = iv_in;
    uint32_t k_i_1_len = 0;

    uint32_t hmac_size;
    uint32_t full_blocks;
    uint32_t leftover;
    uint32_t modulo;
    uint8_t ctr[RLEN + 1];
    uint8_t *key_out_last;

    if (opts->ctr_rlen > RLEN)
    {
        MBCRYPT_FUNCTION_RET_VAR = MBCRYPT_STATUS_FAIL;
        goto MBCRYPT_FUNCTION_EXIT;
    }

    hmac_size = GET_HASH_SIZE_BY_HASH_TYPE(hash_type);

    modulo = ((key_out_len % 8) > 1) ? 1 : 0;
    full_blocks = ((key_out_len / 8) + modulo) / hmac_size;
    leftover = ((key_out_len / 8) + modulo) % hmac_size;

    key_out_last = key_out;

    if (iv_in_len > 0)
    {
        k_i_1_len = iv_in_len;
    }
    for (uint32_t i = 1; i <= full_blocks; ++i, key_out += hmac_size)
    {
        PUT_UINT32_BE(i, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));
        if ((opts->ctr_rpos == -1) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }
        if ((k_i_1_len > 0) && 
            (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, k_i_1, k_i_1_len) != MBCRYPT_STATUS_OK))
        {
            goto MBCRYPT_FUNCTION_EXIT;
        }

        if ((opts->ctr_rpos == 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }
        if ((opts->ctr_rpos > 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, opts->ctr_rpos / 8));
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
            if ((opts->ctr_rpos != (int64_t)fixed_input_len) &&
                (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input + opts->ctr_rpos / 8, 
                            fixed_input_len - opts->ctr_rpos / 8) != MBCRYPT_STATUS_OK))
            {
                goto MBCRYPT_FUNCTION_EXIT;
            }
        }
        else
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, fixed_input_len));
        }
        MBCRYPT_CHECK_RES(hmac_final(ctx, key_out));

        k_i_1 = key_out;
        k_i_1_len = hmac_size;
        key_out_last = key_out;
    }
    if (leftover)
    {
        uint8_t last_block[HMAC_MAX_OUTPUT];

        PUT_UINT32_BE(full_blocks + 1, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));
        if ((opts->ctr_rpos == -1) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }
        // K(i) := PRF (KI, I_V | Ki-1)
        if ((k_i_1_len > 0) && 
            (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, k_i_1, k_i_1_len) != MBCRYPT_STATUS_OK))
        {
            goto MBCRYPT_FUNCTION_EXIT;
        }
        if ((opts->ctr_rpos == 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }
        if ((opts->ctr_rpos > 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, opts->ctr_rpos / 8));
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
            if ((opts->ctr_rpos != (int64_t)fixed_input_len) &&
                (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input + opts->ctr_rpos / 8, 
                            fixed_input_len - opts->ctr_rpos / 8) != MBCRYPT_STATUS_OK))
            {
                goto MBCRYPT_FUNCTION_EXIT;
            }
        }
        else
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, fixed_input_len));
        }
        MBCRYPT_CHECK_RES(hmac_final(ctx, last_block));

        MBCRYPT_CHECK_VALID_NOT_NULL(memcpy(key_out, last_block, leftover));
        MBCRYPT_CHECK_VALID_NOT_NULL(memset(last_block, 0xFF, HMAC_MAX_OUTPUT));

        if (modulo)
        {
            key_out[leftover - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }
    else
    {
        if (modulo)
        {
            key_out_last[hmac_size - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }

MBCRYPT_FUNCTION_EXIT:
    MBCRYPT_FUNCTION_RETURN;
}

static uint32_t _mbcrypt_kbkdf_double_pipeline(mbcrypt_hash_type_e hash_type, 
                                       mbcrypt_hmac_callbacks_t *cbs,
                                       const uint8_t *key_in, const uint32_t key_in_len,
                                       uint8_t *fixed_input, const uint32_t fixed_input_len,
                                       uint8_t *key_out, const uint32_t key_out_len,
                                       mbcrypt_kbkdf_opts_t *opts)
{
MBCRYPT_FUNCTION_BEGIN;

    mbcrypt_hmac_init_t hmac_init = cbs->hmac_init;
    mbcrypt_hmac_update_t hmac_update = cbs->hmac_update;
    mbcrypt_hmac_final_t hmac_final = cbs->hmac_final;
    void *ctx = cbs->hmac_ctx;

    uint32_t hmac_size;
    uint32_t full_blocks;
    uint32_t leftover;
    uint32_t A_len;
    uint32_t modulo;

    uint8_t A[HMAC_MAX_OUTPUT];
    uint8_t ctr[RLEN + 1];
    uint8_t *key_out_last;
    void *A_i_1;
    void *A_i;

    if (opts->ctr_rlen > RLEN)
    {
        MBCRYPT_FUNCTION_RET_VAR = MBCRYPT_STATUS_FAIL;
        goto MBCRYPT_FUNCTION_EXIT;
    }

    A_i_1 = fixed_input;
    A_len = fixed_input_len;

    A_i = &A[0];

    hmac_size = GET_HASH_SIZE_BY_HASH_TYPE(hash_type);

    modulo = ((key_out_len % 8) > 1) ? 1 : 0;
    full_blocks = ((key_out_len / 8) + modulo) / hmac_size;
    leftover = ((key_out_len / 8) + modulo) % hmac_size;

    key_out_last = key_out;

    for (uint32_t i = 1; i <= full_blocks; ++i, key_out += hmac_size)
    {
        PUT_UINT32_BE(i, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));
        MBCRYPT_CHECK_RES(hmac_update(ctx, A_i_1, A_len));
        MBCRYPT_CHECK_RES(hmac_final(ctx, A_i));

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));
        MBCRYPT_CHECK_RES(hmac_update(ctx, A_i, hmac_size));

        if ((opts->ctr_rpos == -1) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }

        if ((opts->ctr_rpos == 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }

        if ((opts->ctr_rpos > 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, opts->ctr_rpos / 8));
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));

            if ((opts->ctr_rpos != (int64_t)fixed_input_len) &&
                (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input +
                            opts->ctr_rpos / 8,
                            fixed_input_len - opts->ctr_rpos / 8) != MBCRYPT_STATUS_OK))
            {
                goto MBCRYPT_FUNCTION_EXIT;
            }
        }
        else
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, fixed_input_len));
        }
        MBCRYPT_CHECK_RES(hmac_final(ctx, key_out));

        A_i_1 = &A[0];
        A_len = hmac_size;
        key_out_last = key_out;
    }
    if (leftover)
    {
        uint8_t last_block[HMAC_MAX_OUTPUT];

        PUT_UINT32_BE(full_blocks + 1, ctr, 0);
        ctr[4] = 0;

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));
        MBCRYPT_CHECK_RES(hmac_update(ctx, A_i_1, A_len));
        {
            goto MBCRYPT_FUNCTION_EXIT;
        }
        MBCRYPT_CHECK_RES(hmac_final(ctx, A_i));

        MBCRYPT_CHECK_RES(hmac_init(ctx, key_in, key_in_len));

        if ((opts->ctr_rpos == -1) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }
        MBCRYPT_CHECK_RES(hmac_update(ctx, A_i, A_len));

        if ((opts->ctr_rpos == 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
        }

        if ((opts->ctr_rpos > 0) &&
            (opts->ctr_rlen > 0))
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, opts->ctr_rpos / 8));
            MBCRYPT_CHECK_RES(hmac_update(ctx, ctr + (RLEN - opts->ctr_rlen), opts->ctr_rlen));
            if ((opts->ctr_rpos != (int64_t)fixed_input_len) &&
                (MBCRYPT_FUNCTION_RET_VAR = hmac_update(ctx, fixed_input + opts->ctr_rpos / 8, 
                            fixed_input_len - opts->ctr_rpos / 8) != MBCRYPT_STATUS_OK))
            {
                goto MBCRYPT_FUNCTION_EXIT;
            }
        }
        else
        {
            MBCRYPT_CHECK_RES(hmac_update(ctx, fixed_input, fixed_input_len));
        }
        MBCRYPT_CHECK_RES(hmac_final(ctx, last_block));

        MBCRYPT_CHECK_VALID_NOT_NULL(memcpy(key_out, last_block, leftover));
        MBCRYPT_CHECK_VALID_NOT_NULL(memset(last_block, 0xFF, HMAC_MAX_OUTPUT));

        if (modulo)
        {
            key_out[leftover - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }
    else
    {
        if (modulo)
        {
            key_out_last[hmac_size - 1] &= BIT_MASK_LEFT(key_out_len % 8);
        }
    }

MBCRYPT_FUNCTION_EXIT:
    MBCRYPT_FUNCTION_RETURN;
}

mbcrypt_status_e kbkdf(mbcrypt_kbkdf_mode_e mode, mbcrypt_hash_type_e hash_type,
                        mbcrypt_hmac_callbacks_t *cbs,
                        const uint8_t *key_in, const uint32_t key_in_len,
                        const uint8_t *iv_in, const uint32_t iv_in_len,
                        uint8_t *fixed_input, const uint32_t fixed_input_len,
                        uint8_t *key_out, const uint32_t key_out_len,
                        mbcrypt_kbkdf_opts_t *opts)
{
MBCRYPT_FUNCTION_BEGIN;

    mbcrypt_kbkdf_opts_t tmp_opts =
        {
            .ctr_rlen = 4,
            .ctr_rpos = 0
        };
    MBCRYPT_CHECK_VALID_NOT_NULL(key_in);
    MBCRYPT_CHECK_VALID_NOT_NULL(iv_in);
    MBCRYPT_CHECK_VALID_NOT_NULL(fixed_input);
    MBCRYPT_CHECK_VALID_NOT_NULL(key_out);

    MBCRYPT_CHECK_VALID_NOT_NULL(cbs->hmac_final);
    MBCRYPT_CHECK_VALID_NOT_NULL(cbs->hmac_init);
    MBCRYPT_CHECK_VALID_NOT_NULL(cbs->hmac_update);

    if (opts != NULL)
    {
        tmp_opts.ctr_rlen = opts->ctr_rlen;
        tmp_opts.ctr_rpos = opts->ctr_rpos;
    }

    switch (mode)
    {
    case MBCRYPT_KBKDF_MODE_COUNTER:
        MBCRYPT_CHECK_RES(_mbcrypt_kbkdf_counter(hash_type, cbs, key_in, key_in_len, fixed_input,
                                fixed_input_len, key_out, key_out_len, &tmp_opts));
        break;
    case MBCRYPT_KBKDF_MODE_FEEDBACK:
        MBCRYPT_CHECK_RES(_mbcrypt_kbkdf_feedback(hash_type, cbs, key_in, key_in_len, iv_in, iv_in_len,
                                 fixed_input, fixed_input_len, key_out, key_out_len, &tmp_opts));
        break;
    case MBCRYPT_KBKDF_MODE_DOUBLE_PIPELINE:
        MBCRYPT_CHECK_RES(_mbcrypt_kbkdf_double_pipeline(hash_type, cbs, key_in, key_in_len, fixed_input,
                                        fixed_input_len, key_out, key_out_len, &tmp_opts));
        break;
    default:
        MBCRYPT_FUNCTION_RET_VAR = MBCRYPT_STATUS_FAIL_NOT_IMPLEMENTED;
    }
    
MBCRYPT_FUNCTION_EXIT:

#if (MBCRYPT_LEVEL == MAX_MBCRYPT_LEVEL) || (SECURED_KBKDF == ENABLED)

#endif /* SECURED_KBKDF */

    MBCRYPT_FUNCTION_RETURN;
}