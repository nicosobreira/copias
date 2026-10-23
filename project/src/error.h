#pragma once

#include <stdio.h>
#include <stdlib.h>

#define LOG_ERROR(message, ...)                          \
    fprintf(stderr, message __VA_OPT__(, ) __VA_ARGS__); \
    fputc('\n', stderr);

#define EXIT_ERROR(message, ...)    \
    LOG_ERROR(message, __VA_ARGS__) \
    exit(1)
