/**
 * @file ml_defs.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2013-12-31
 */


#ifndef ML_DEFS_H
#define ML_DEFS_H

// Cross platform compatibility
#if defined(WIN32)
#define tl_second_sleep(seconds) Sleep(1000 * seconds) // Seconds to milliseconds
#else
#include <unistd.h>
#define tl_second_sleep(seconds) sleep(seconds)  // No conversion necessary
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <pthread.h>

#include "ml_errors.h"
#include "tlib_debug.h"

#endif

