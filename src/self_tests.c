#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sha1.h"
#include "sha256.h"
#include "sha512.h"


void print_arr(uint8_t *data, uint32_t data_len)
{
    for (int i = 0; i < data_len; i++)
        printf("%02X", data[i]);
    printf("\n");
}

int main() 
{
    printf("%d\n", sha1_selftest());
    return 0;
}