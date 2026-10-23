#include "command_enum.h"
#include "error.h"
#include "utils.h"

#include "commands/cat/cat.h"
#include "commands/parameters.h"
#include "commands/wc/wc.h"

void match_command(Command copy, Parameters *param);

int main(int argc, char *argv[argc])
{
    if (argc == 1)
    {
        EXIT_ERROR("Please, type command");
    }

    Command command = Command_FromName(argv[1]);

    // Don't consider the program path and selected command.
    const int offset = 2;

    int total_args = argc - offset;
    char **args = argv + offset;

    Parameters param = {.total = total_args, .args = args};

    match_command(command, &param);

    return 0;
}

void match_command(Command command, Parameters *param)
{
    switch (command)
    {
    case COMMAND_CAT: {
        cat(param);
        break;
    }
    case COMMAND_WC: {
        wc(param);
        break;
    }
    case COMMAND_HEAD: {
        break;
    }
    case COMMAND_TAIL: {
        break;
    }
    default: {
        EXIT_ERROR("Plese, enter a valid command.");
        break;
    }
    }
}
