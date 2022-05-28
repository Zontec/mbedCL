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
*   @file bignum.h
*   @brief File contains ARIA API functions.
*	@author Zontec
*	@version 1.1
*	@date 2022.08.12
*/

#ifndef BIGNUM_H
#define BIGNUM_H


#include "mbcrypt.h"

#define LIBTOMMATH              ENABLED

#include "tommath.h"

#if (LIBTOMMATH == ENABLED)

typedef mp_int bignum_t;


#endif /* LIBTOMMATH */

#ifdef __cplusplus
extern "C" {
#endif

uint32_t 

/* API is here */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BIGNUM_H */