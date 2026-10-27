#include "internal.h"

#include <stdio.h>
#include <string.h>

int blob_hash_from_buffer(char *out_hex, size_t out_hex_size, char *data, size_t data_len)
{
    // FIX: Estou gerando uma hash, mas não uma boa. Como o buffer tem um único tamanho
    // BUFFER_SIZE, se o header for muito grande, vai ter menos espaço para
    // `data`. Preciso delimitar um tamanho para o HEADER e CONTENT.
    char buffer[BUFFER_SIZE + 1];
    size_t buffer_size = BUFFER_SIZE;

    memset(buffer, 0, sizeof(buffer));

    // NOTE: Can't use the return value of `snprintf` for the buffer_size;
    snprintf(buffer, sizeof(buffer), "blob %zu %.*s", data_len, (int)data_len, data);

    unsigned char digest[HASH_LEN + 1];

    size_t digest_len = sizeof(digest);

    if (hash_string(buffer, buffer_size, digest, &digest_len) == 1)
    {
        return 1;
    }

    bytes_to_hex(out_hex, digest, digest_len);

    return 0;
}
