#pragma once

#include "commom/error.h"
#include "commom/parameters.h"

static inline int cmd_commit(Parameters param)
{
    if (param.total != 0)
    {
        LOG_ERROR("Type one commit message!");
        return 1;
    }
    const char *message = param.args[0];

    return 0;
}
