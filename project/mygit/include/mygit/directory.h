#pragma once

#define GIT_DIR_PATH "./.mygit"
#define OBJECT_DIR_PATH \
    GIT_DIR_PATH "/"    \
                 "objects"

#ifdef _WIN32

#include <direct.h>
#define CREATE_DIR(path) _mkdir(path)

#else

#include <sys/stat.h>
#define CREATE_DIR(path) mkdir(path, 0755)

#endif
