#pragma once

/** Generator macro for copies
 * @def COPIES(X)
 *
 * @param X `X(enum_name, command_name)`
 */
#define COMMANDS(X)       \
    X(COMMAND_CAT, cat)   \
    X(COMMAND_WC, wc)     \
    X(COMMAND_HEAD, head) \
    X(COMMAND_TAIL, tail)
