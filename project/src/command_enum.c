#include "command_enum.h"

#include <string.h>

#include "utils.h"

#define X(enum_name, command_name)                           \
    if (strcmp(command, WRAP_QUOTES_VAL(command_name)) == 0) \
    {                                                        \
        return enum_name;                                    \
    }
Command Command_FromName(char *command)
{
    COMMANDS(X)

    return COMMAND_NULL;
}
#undef X
