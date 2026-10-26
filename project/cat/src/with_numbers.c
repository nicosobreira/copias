#include "with_numbers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commom/buffer.h"
#include "commom/error.h"

static inline int get_line(char *buffer, size_t *len, FILE *file)
{
    const char delimiter = '\n';

    size_t current_index = 0;

    char c;
    while ((c = (char)fgetc(file)) != EOF)
    {
        if (current_index >= (*len))
        {
            *len += 1;
            buffer = realloc(buffer, *len);
        }

        buffer[current_index] = c;
        current_index += 1;

        if (c == delimiter)
        {
            break;
        }
    }

    if (feof(file))
    {
        return 1;
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading file");
        return 2;
    }

    // Need to add the '\0' at the end of buffer
    current_index += 1;
    if (current_index >= (*len))
    {
        *len += 1;
        buffer = realloc(buffer, *len);
    }
    buffer[current_index] = '\0';

    return 0;
}

static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    char *buffer = malloc(BUFFER_SIZE);

    size_t buffer_size;
    while ((get_line(buffer, &buffer_size, file) == 0))
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(buffer, stream);

        total_lines += 1;

        memset(buffer, 0, BUFFER_SIZE);
    }

    free(buffer);

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
