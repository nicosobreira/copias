#include "cat.h"

#include <stdio.h>
#include <string.h>

#include "normal.h"
#include "with_numbers.h"

static inline void cat_help(void)
{
    printf("Usage: cat [OPTION] [FILE(s)]\n"
           "Concatenate FILE(s) to standard output.\n"
           "\n"
           "Needs at least one file to concatenate.\n"
           "  -h, --help      display this help and exit\n"
           "  -n, --number    number all output lines\n");
}

void cat(Parameters *param)
{
    if (strcmp(param->args[0], "--help") == 0)
    {
        cat_help();
        return;
    }
    if (strcmp(param->args[0], "-n") == 0 || strcmp(param->args[0], "--number") == 0)
    {
        Parameters_Shift(param);

        show_files_number(*param, stdout);
        return;
    }

    show_files(*param, stdout);
}
