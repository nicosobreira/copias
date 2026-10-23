#include "normal.h"

#include "buffer.h"
#include "error.h"

static inline void output_file(FILE *file, FILE *stream)
{
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        fputs(buffer, stream);
    }
}

static inline void show_file(const char *file_path, FILE *stream)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        LOG_ERROR("Failed to open the file \"%s\"", file_path);
        return;
    }

    output_file(file, stream);

    fclose(file);
}

void show_files(Parameters param, FILE *stream)
{
    for (int i = 0; i < param.total; i++)
    {
        show_file(param.args[i], stream);
    }
}
