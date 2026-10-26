#include "commom/parameters.h"

void Parameters_Shift(Parameters *param)
{
    if (Parameters_IsEmpty(param))
    {
        return;
    }

    param->args += 1;
    param->total -= 1;
}

Parameters Parameters_FromArgv(int argc, char **argv)
{
    // Ignore executable name
    const int offset = 1;

    return (Parameters){
        .total = argc - offset,
        .args = argv + offset,
    };
}
