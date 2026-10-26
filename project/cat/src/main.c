#include "cat.h"

int main(int argc, char **argv)
{
    Parameters param = Parameters_FromArgv(argc, argv);

    cat(&param);

    return 0;
}
