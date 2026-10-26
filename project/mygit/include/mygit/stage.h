#pragma once

#include <stdio.h>

#include "commom/buffer.h"
#include "commom/error.h"
#include "mygit/directory.h"
#include "mygit/hash.h"
#include "string.h"

/* dst have to be in append mode! */
static inline int copy_file(FILE *src, FILE *dst)
{
    char buffer[BUFFER_SIZE_LARGE];

    size_t bytes;
    while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), src)) > 0)
    {
        if (fwrite(buffer, sizeof(char), bytes, dst) == 0)
        {
            LOG_ERROR("Error while writing the file.");
            // FIX: Clear all changes made by `fwrite`.
            return 1;
        }
    }

    if (ferror(src))
    {
        LOG_ERROR("Error on src.");
        return 1;
    }

    return 0;
}

static inline int save_blob(FILE *index, const char *hash, size_t len, FILE *file_path)
{
    const char dir[2 + 1] = {hash[0], hash[1], '\0'};

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s/%s", OBJECT_DIR_PATH, dir);

    CREATE_DIR(buffer);

    const char *out_file = hash + 2;

    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "/%.*s", (int)(len - 2), out_file);

    FILE *out = fopen(buffer, "a");
    if (out == NULL)
    {
        LOG_ERROR("Can't create file '%s'", buffer);
        return 1;
    }

    int result = copy_file(file_path, out);
    fclose(out);
    if (result != 1)
    {
        return 1;
    }

    return 0;
}

static inline int stage_file(FILE *index, FILE *file_path)
{
    char hash[HASH_LEN_HEX];
    if (hash_blob(hash, sizeof(hash), file_path) != 0)
    {
        LOG_ERROR("Couldn't hash the file");
        return 1;
    }

    if (save_blob(index, hash, sizeof(hash), file_path) != 1)
    {
        return 1;
    }

    fprintf(index, "blob %.*s\n", (int)sizeof(hash), hash);

    return 0;
}
