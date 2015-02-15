/**
 * @file tlib_debug.h
 * @brief Debugging functions.
 * @author Travis Lane
 * @version 0.4.1
 * @date 20.4.15-18
 */


#ifndef TLIB_DEBUG_H
#define TLIB_DEBUG_H

#include <stdio.h>

#ifndef NDEBUG
#define TRACE(...) fprintf(stderr, __VA_ARGS__)
#else
#define TRACE(...)
#endif

#define WARNING(...) fprintf(stderr, __VA_ARGS__)

#endif
