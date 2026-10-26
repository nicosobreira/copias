#include "internal.h"

#include <stdio.h>

#define HEX_CHARS_PER_BYTE 2

void bytes_to_hex(char *out_hex, unsigned char *digest, size_t digest_len)
{
    char *cursor = out_hex;

    for (size_t i = 0; i < digest_len; i++)
    {
        snprintf(cursor, HEX_CHARS_PER_BYTE + 1, "%02x", digest[i]);
        cursor += HEX_CHARS_PER_BYTE;
    }
}
