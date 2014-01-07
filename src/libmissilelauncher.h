/**
 * @file libmissilelauncher.h
 * @brief Header file for libmissilelauncher.h
 * Use these functions to control your missile launcher
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

/// Use this enumeration to determine the type of launcher. 
typedef enum ml_launcher_type {
  ML_NOT_LAUNCHER, ///< The object isn't a launcher
  ML_STANDARD_LAUNCHER ///< The object is a standard thunder launcher
} ml_launcher_type;

/// Use this enumerationt to specify which direction to move the launcher in
typedef enum ml_launcher_direction {
  ML_DOWN, ///< Aim the launcher downwards
  ML_UP,   ///< Aim the launcher upwards
  ML_LEFT, ///< Move the launcher to the left
  ML_RIGHT ///< Move the launcher to the righ
} ml_launcher_direction;

typedef struct ml_launcher_t ml_launcher_t; 

// ********** API Functions **********
// Library init
int16_t ml_init_library();
int16_t ml_cleanup_library();
uint8_t ml_is_library_init();
// Polling
int16_t ml_start_continuous_poll();
int16_t ml_stop_continuous_poll();
uint8_t ml_is_polling();
uint8_t ml_get_poll_rate();
int16_t ml_set_poll_rate(uint8_t);
// Launcher arrays
int16_t ml_get_launcher_array(ml_launcher_t ***, uint32_t *);
int16_t ml_free_launcher_array(ml_launcher_t **);
// Launcher refrence
int16_t ml_reference_launcher(ml_launcher_t *);
int16_t ml_dereference_launcher(ml_launcher_t *);
// Launcher control
ml_launcher_type ml_get_launcher_type(ml_launcher_t *);
int16_t ml_fire_launcher(ml_launcher_t *);
int16_t ml_move_launcher_degrees(ml_launcher_t *, ml_launcher_direction, uint16_t);
int16_t ml_move_launcher_mseconds(ml_launcher_t *, ml_launcher_direction, uint32_t);
int16_t ml_zero_launcher(ml_launcher_t *);
int16_t ml_led_on(ml_launcher_t *);
int16_t ml_led_off(ml_launcher_t *);
uint8_t ml_get_led_stat(ml_launcher_t *);
#endif
