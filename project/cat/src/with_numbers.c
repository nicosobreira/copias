#include "with_numbers.h"

#include <stdio.h>
#include <stdlib.h>

#include "commom/buffer.h"
#include "commom/error.h"

/*
static inline size_t my_getline(char **buffer, size_t *size, FILE *stream)
{
    const char delimiter = '\n';

    size_t i = 0;
    char c;
    while ((c = (char)fgetc(stream)) != delimiter)
    {
        if (c == '\0')
        {
            break;
        }

        if (i >= *size)
        {
            *size = i + 1;
            *buffer = realloc(*buffer, *size);
        }

        *buffer[i] = c;
        i += 1;
    }

    *buffer[i + 1] = '\0';

    return i + 1;
}
*/

static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    char *line = malloc(BUFFER_SIZE);

    size_t line_size;
    while ((line_size = getline(&line, &(size_t){BUFFER_SIZE}, file) != -1))
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(line, stream);

        total_lines += 1;
    }

    free(line);

    return total_lines;
}

static inline int show_file_number(const char *file_path, FILE *stream, int total_lines)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        LOG_ERROR("Failed to open the file \"%s\"", file_path);

        return total_lines;
    }

    int new_total = output_file_number(file, stream, total_lines);

    fclose(file);

    return new_total;
}

void show_files_number(Parameters param, FILE *stream)
{
    int total_lines = 1;
    for (int i = 0; i < param.total; i++)
    {
        total_lines = show_file_number(param.args[i], stream, total_lines);
    }
}
