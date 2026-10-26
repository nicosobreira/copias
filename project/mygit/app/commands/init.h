#pragma once

#include "mygit/directory.h"

#include "commom/error.h"
#include "commom/parameters.h"

static inline int cmd_init(Parameters param)
{
    if (param.total != 0)
    {
        LOG_ERROR("Type no arguments, please.");
        return 1;
    }

    if (CREATE_DIR(GIT_DIR_PATH) != 0)
    {
        LOG_ERROR("This folder already exists!");
        return 1;
    }

    if (CREATE_DIR(OBJECT_DIR_PATH) != 0)
    {
        LOG_ERROR("This folder already exists!");
        return 1;
    }

    printf("The folder .mygit was created!");

    return 0;
}
