/* File: tlib_debug.h
 * Generic debugging definitions
 * Author: Travis Lane
 * Last Modified Aug 23rd, 2013
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
