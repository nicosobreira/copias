#include "wc.h"

#include <stdio.h>
#include <string.h>

#include "commom/error.h"

typedef struct Data
{
    int lines;
    int words;
    int bytes;
} Data;

Data Data_New(void)
{
    return (Data){
        .lines = 0,
        .words = 0,
        .bytes = 1, // The '\0' is at the end of every file
    };
}

void Data_Sum(Data *result, Data plus)
{
    result->lines += plus.lines;
    result->words += plus.words;
    result->bytes += plus.bytes;
}

static inline void wc_help(void)
{
    printf("Usage: cat [OPTION] [FILES(s)]\n"
           "Show the number of new lines, words and bytes.\n"
           "  --help    display this help page and exit\n");
}

void Data_Print(FILE *stream, Data data, const char *file_path)
{
    if (file_path == NULL)
    {
        fprintf(stream, "%4d  %4d  %4d\n", data.lines, data.words, data.bytes);
    }
    else
    {
        fprintf(stream, "%4d  %4d  %4d  %s\n", data.lines, data.words, data.bytes, file_path);
    }
}

static inline bool is_delimiter(char c)
{
    return c == ' ' || c == '\n';
}

static inline bool is_character(char c)
{
    return !is_delimiter(c);
}

Data count_file(FILE *file)
{
    Data data = Data_New();
    char previous_c = (char)fgetc(file);
    if (previous_c == EOF)
    {
        data.lines += 1;
        return data;
    }

    char c;
    while ((c = (char)fgetc(file)) != EOF)
    {
        if (c == '\n')
        {
            data.lines += 1;
        }

        if (is_character(previous_c) && is_delimiter(c))
        {
            data.words += 1;
        }

        data.bytes += 1;

        previous_c = c;
    }

    return data;
}

Data do_file_path(FILE *file, const char *path, FILE *stream)
{
    Data data = count_file(file);

    Data_Print(stream, data, path);

    return data;
}

void do_file(FILE *file, FILE *stream)
{
    Data data = count_file(file);

    Data_Print(stream, data, NULL);
}

void do_files(Parameters param, FILE *stream)
{
    Data total = {0};

    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("No such file \"%s\"", file_path);

            continue;
        }

        Data data = do_file_path(file, file_path, stream);

        Data_Sum(&total, data);

        fclose(file);
    }

    Data_Print(stream, total, "  total");
}

void do_stdin(void)
{
    do_file(stdin, stdout);
}

void wc(Parameters *param)
{
    if (Parameters_IsEmpty(param))
    {
        do_stdin();

        return;
    }

    if (strcmp(param->args[0], "--help") == 0)
    {
        wc_help();

        return;
    }
    if (strcmp(param->args[0], "--bytes") == 0 || strcmp(param->args[0], "-c") == 0)
    {
        return;
    }
    if (strcmp(param->args[0], "--lines") == 0 || strcmp(param->args[0], "-l") == 0)
    {
        return;
    }
    if (strcmp(param->args[0], "--words") == 0 || strcmp(param->args[0], "-w") == 0)
    {
        return;
    }

    do_files(*param, stdout);
}
