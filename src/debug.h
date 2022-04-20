#ifndef DEBUG_H
#define DEBUG_H 1

#include <stdio.h>


// Debugging macros
#ifdef DEBUG
#   define dlog( ...) fprintf(stderr, __VA_ARGS__)
#else
#   define dlog(...)
#endif

#endif
