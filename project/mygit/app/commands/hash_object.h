#pragma once

#include "mygit/hash.h"

#include "commom/error.h"
#include "commom/parameters.h"

static inline int cmd_hash_object(Parameters param)
{
    if (param.total != 1)
    {
        LOG_ERROR("Type the correct number of inputs");
        return 1;
    }

    const char *file_path = param.args[0];

    FILE *input = fopen(file_path, "rb");

    if (input == NULL)
    {
        LOG_ERROR("Unable to open the file '%s'", file_path);
        return 1;
    }

    char hash[HASH_LEN_HEX];
    if (hash_blob(hash, sizeof(hash), input) == 1)
    {
        LOG_ERROR("Error");

        fclose(input);
        return 1;
    }

    printf("%.*s\n", (int)sizeof(hash), hash);

    fclose(input);
    return 0;
}
