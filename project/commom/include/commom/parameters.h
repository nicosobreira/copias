#pragma once

#include <stdbool.h>

typedef struct Parameters
{
    int total;
    char **args;
} Parameters;

static inline bool Parameters_IsEmpty(Parameters *param)
{
    return param->total <= 0;
}

void Parameters_Shift(Parameters *param);

Parameters Parameters_FromArgv(int argc, char **argv);
