#include "wc.h"

int main(int argc, char **argv)
{
    Parameters param = Parameters_FromArgv(argc, argv);

    wc(&param);

    return 0;
}
