#pragma once

#include <string.h>

#include "commom/error.h"
#include "commom/parameters.h"

#include "mygit/stage.h"

// STAGE PROCESS
// 1. Turn each

static inline int cmd_add(Parameters param)
{
    if (param.total == 0)
    {
        LOG_ERROR("Nothing specified, nothing added.");
        return 1;
    }

    if (strcmp(param.args[0], ".") == 0)
    {
        LOG_ERROR("Not implemented yet :(");
        return 2;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s/index", GIT_DIR_PATH);
    FILE *index = fopen(buffer, "a");

    char *file_name;
    for (int i = 0; i < param.total; i++)
    {
        file_name = param.args[i];
        FILE *file = fopen(file_name, "r");
        if (file == NULL)
        {
            LOG_ERROR("Unable to open '%s'", file_name);
            continue;
        }

        if (stage_file(index, file) != 0)
        {
            LOG_ERROR("Unable to stage file '%s'", file_name);
            continue;
        }

        fclose(file);
    }

    fclose(index);

    return 0;
}
