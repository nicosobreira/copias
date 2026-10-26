#include "normal.h"

#include "commom/error.h"

static inline void output_file(FILE *file, FILE *stream)
{
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        fputc(c, stream);
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
