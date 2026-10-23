#pragma once

typedef struct Parameters
{
    int total;
    char **args;
} Parameters;

static inline bool Parameters_IsEmpty(Parameters *param)
{
    return param->total <= 0;
}

static inline void Parameters_Shift(Parameters *param)
{
    if (Parameters_IsEmpty(param))
    {
        return;
    }

    param->args += 1;
    param->total -= 1;
}
