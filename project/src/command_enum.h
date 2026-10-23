#pragma once

#include "commands_macro.h"

#define X(enum_name, command_name) enum_name,
typedef enum Command
{
    COMMAND_NULL,
    COMMANDS(X)
    //
    COMMAND_TOTAL_COUNT,

} Command;
#undef X

Command Command_FromName(char *command);
