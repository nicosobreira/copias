#include <stdio.h>
#include <string.h>

#include "commom/error.h"
#include "commom/parameters.h"

#include "commands/add.h"
#include "commands/commit.h"
#include "commands/hash_object.h"
#include "commands/init.h"

typedef int (*CommandFunc)(Parameters param);

typedef struct Command
{
    const char *name;
    CommandFunc run;
} Command;

static Command commands[] = {
    {
        .name = "hash-object",
        .run = cmd_hash_object,
    },
    {
        .name = "init",
        .run = cmd_init,
    },
    {
        .name = "add",
        .run = cmd_add,
    },
    {
        .name = "commit",
        .run = cmd_commit,
    },
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        LOG_ERROR("Not enough arguments");
        return 1;
    }

    const char *command = argv[1];

    const size_t commands_len = sizeof(commands) / sizeof(commands[0]);
    for (size_t i = 0; i < commands_len; i++)
    {
        if (strcmp(command, commands[i].name) == 0)
        {
            Parameters param = {
                .total = argc - 2,
                .args = argv + 2,
            };
            return commands[i].run(param);
        }
    }

    EXIT_ERROR("mygit: '%s' is not a command\n", command);

    return 0;
}
