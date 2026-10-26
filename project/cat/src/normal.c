#include "normal.h"

#include "commom/error.h"

#include <stdio.h>

static inline void output_file(FILE *file, FILE *stream)
{
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        int did_write = fputc(c, stream);

        if (did_write == EOF && ferror(stream))
        {
            LOG_ERROR("I/O error while writing from stream");
            return;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading from file");
    }
}

void show_files(Parameters param, FILE *stream)
{
    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("Failed to open the file \"%s\"", file_path);
            continue;
        }

        output_file(file, stream);

        fclose(file);
    }
}
