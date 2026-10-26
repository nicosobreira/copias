#pragma once

#include "mygit/hash.h"

#include <string.h>

#include "commom/buffer.h"
#include "commom/error.h"

static inline int hash_string(const char *string, size_t len, unsigned char out[HASH_LEN], size_t *out_len)
{
    if (!EVP_Q_digest(NULL, HASH_ALGO, NULL, string, len, out, out_len))
    {
        LOG_ERROR("Failed to compute " HASH_ALGO);
        return 1;
    }

    return 0;
}

static inline int hash_stream(FILE *f, unsigned char out[HASH_LEN], unsigned int *out_len)
{
    int ok = 0;
    EVP_MD *md = EVP_MD_fetch(NULL, HASH_ALGO, NULL);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    unsigned char buffer[BUFFER_SIZE_LARGE];
    size_t lines;

    if (!md || !ctx || !EVP_DigestInit_ex2(ctx, md, NULL))
        goto end;

    while ((lines = fread(buffer, sizeof(char), sizeof(buffer), f)) > 0)
    {
        if (!EVP_DigestUpdate(ctx, buffer, lines))
            goto end;
    }

    if (ferror(f))
        goto end;

    ok = EVP_DigestFinal_ex(ctx, out, out_len);
end:
    EVP_MD_CTX_free(ctx);
    EVP_MD_free(md);
    return ok;
}

/* hash_hex.c */
void bytes_to_hex(char *out_hex, unsigned char *digest, size_t digest_len);

/* blob_hash.c */
int blob_hash_from_buffer(char *out_hex, size_t out_hex_size, char *data, size_t data_len);
