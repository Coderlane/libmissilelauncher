/**
 * @file libmissilelauncher.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.3
 * @date 2014-01-02
 */

#ifndef LIBMISSILELAUNCHER_H
#define LIBMISSILELAUNCHER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <pthread.h>

#include "ml_errors.h"

// ********** Controller Definitions **********
#define ML_DEFAULT_POLL_RATE 2
#define ML_MAX_POLL_RATE 120
#define ML_MIN_POLL_RATE 1

typedef void(*ml_void_event_handler)();
typedef struct ml_controller_t ml_controller_t;

// ********** Launcher Definitions **********
#define ML_STD_VENDOR_ID 8483
#define ML_STD_PRODUCT_ID 4112

typedef enum ml_launcher_type {
  ML_NOT_LAUNCHER,
  ML_STANDARD_LAUNCHER
} ml_launcher_type;

typedef struct ml_launcher_t ml_launcher_t; 

// ********** API Functions **********
int16_t ml_init_library();
int16_t ml_cleanup_library();
uint8_t ml_is_library_init();

int16_t ml_start_continuous_poll();
int16_t ml_stop_continuous_poll();
uint8_t ml_is_polling();
uint8_t ml_get_poll_rate();
int16_t ml_set_poll_rate(uint8_t);

int16_t ml_get_launcher_array(ml_launcher_t ***);
int16_t ml_free_launcher_array(ml_launcher_t **);


int16_t ml_refrence_launcher(ml_launcher_t *);
int16_t ml_derefrence_launcher(ml_launcher_t *);
/*
int16_t ml_file_launcher(ml_launcher_t);
int16_t ml_move_launcher(ml_launcher_t, ml_launcher_direcion, uint16_t);
int16_t ml_zero_launcher(ml_launcher_t);
*/
#endif
