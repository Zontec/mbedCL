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
*   @file mbcrypt_utils.h
*   @brief File contains utils for library functions.
*	@author Zontec
*	@version 1.1
*	@date 2022.07.02
*/

#ifndef MBCRYPT_UTILS_H
#define MBCRYPT_UTILS_H

#include "mbcrypt.h"

#ifdef __cplusplus
extern "C" {
#endif

void mem_xor_secured(uint8_t *dst, const uint8_t *src, uint32_t size);

uint32_t is_le();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MBCRYPT_UTILS_H */