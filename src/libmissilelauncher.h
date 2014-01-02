/**
 * @file libmissilelauncher.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.3
 * @date 2014-01-02
 */

#ifndef LIBMISSILELAUNCHER_H
#define LIBMISSILELAUNCHER_H

// Cross platform compatibility
#if defined(WIN32)
#define ml_second_sleep(seconds) Sleep(1000 * seconds) // Seconds to milliseconds
#else
#include <unistd.h>
#define ml_second_sleep(seconds) sleep(seconds)  // No conversion necessary
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <pthread.h>

#include "ml_errors.h"
#include "tlib_debug.h"

// ********** Controller Definitions **********
#define ML_MAX_ATTACHED_DEVICES 256

#define ML_DEFAULT_POLL_RATE 2
#define ML_MAX_POLL_RATE 120
#define ML_MIN_POLL_RATE 1

// ***** Typedef *****
typedef void(*ml_void_event_handler)();
typedef struct ml_controller_t ml_controller_t;

// ********** Launcher Definitions **********
#define ML_STD_VENDOR_ID 8483
#define ML_STD_PRODUCT_ID 4112
#define ML_INITIAL_LAUNCHER_ARRAY_SIZE 10

typedef enum ml_launcher_type {
  ML_NOT_LAUNCHER,
  ML_STANDARD_LAUNCHER
} ml_launcher_type;

typedef struct ml_launcher_t ml_launcher_t; 

typedef struct ml_arr_launcher_t ml_arr_launcher_t;

// ***** Global Statics *****
static ml_controller_t __attribute__ ((unused)) *ml_main_controller = NULL;
static pthread_mutex_t __attribute__ ((unused)) ml_main_controller_mutex = PTHREAD_MUTEX_INITIALIZER;

// ***** Functions *****
int16_t ml_init_library();
int16_t ml_cleanup_library();
uint8_t ml_is_library_init();

int16_t ml_start_continuous_poll();
int16_t ml_stop_continuous_poll();
uint8_t ml_is_polling();
uint8_t ml_get_poll_rate();
int16_t ml_set_poll_rate(uint8_t poll_rate_seconds);

int16_t _ml_init_controller(ml_controller_t *controller);
int16_t _ml_cleanup_controller(ml_controller_t *controller);

void *_ml_poll_for_launchers(void *target_arg);

#endif
