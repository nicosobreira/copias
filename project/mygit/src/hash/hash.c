#include "mygit/hash.h"

#include <stdio.h>
#include <stdlib.h>

#include "commom/buffer.h"
#include "commom/error.h"

#include "internal.h"

int hash_blob(char *out_hex, size_t out_hex_size, FILE *input)
{
    char data[BUFFER_SIZE];

    memset(data, 0, sizeof(data));

    size_t data_len = fread(data, sizeof(char), sizeof(data), input);

    if (ferror(input))
    {
        LOG_ERROR("I/O error when reading file");
        return 1;
    }

    // FIX: Tenho que passar o TAMANHO DO ARQUIVO para a função
    // `blob_hash_from_buffer`. Assim, arquivos muito grandes vão possuir uma
    // hash melhor
    if (blob_hash_from_buffer(out_hex, out_hex_size, data, data_len) == 1)
    {
        LOG_ERROR("Error");
        return 1;
    }

    return 0;
}
