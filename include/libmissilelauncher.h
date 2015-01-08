/**
 * @file libmissilelauncher.h
 * @brief Header file for libmissilelauncher.h
 * Use these functions to control your missile launcher
 * @author Travis Lane
 * @version 0.3.0
 * @date 2014-05-18
 */

#ifndef LIBMISSILELAUNCHER_H
#define LIBMISSILELAUNCHER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ********** Controller Definitions **********
#define ML_DEFAULT_POLL_RATE 2 ///< 2 Seconds is the default polling rate
#define ML_MAX_POLL_RATE 120 ///< 120 Seconds or 2 minutes is the maximum wait between polling
#define ML_MIN_POLL_RATE 1 ///< 1 Second is the minimum wait between polling

typedef void(*ml_void_event_handler)
(); ///< A future object, currently not implemented.
typedef struct ml_controller_t
    ml_controller_t; ///< Controller typedef, Don't use this object directly.

// ********** Launcher Definitions **********
#define ML_STD_VENDOR_ID 8483 ///< The Vendor ID of a standard launcher
#define ML_STD_PRODUCT_ID 4112 ///< The Product ID of a standard launcer

/// Use this enumeration to determine the type of launcher.
typedef enum ml_launcher_type {
  ML_NOT_LAUNCHER, ///< The object isn't a launcher
  ML_STANDARD_LAUNCHER ///< The object is a standard thunder launcher
} ml_launcher_type;

/// Use this enumerationt to specify which direction to move the launcher in
typedef enum ml_launcher_direction {
  ML_DOWN, ///< Aim the launcher downwards
  ML_UP,	 ///< Aim the launcher upwards
  ML_LEFT, ///< Move the launcher to the left
  ML_RIGHT ///< Move the launcher to the righ
} ml_launcher_direction;

typedef struct ml_launcher_t
    ml_launcher_t; ///< Launcher typedef, Don't use this object directly.

///< Error codes used in the library an enumeration is used
typedef enum ml_error_codes {
  ML_OK, ///< Everything is OK
  ML_NOT_IMPLEMENTED, ///< Specific function not implemented.
  ML_TEST_ERROR, ///< A test failed.
  ML_LIBRARY_ALREADY_INIT, ///< Library was already initialized. ml_init_library called more than once.
  ML_LIBRARY_NOT_INIT, ///< Library was not initialized, call ml_init_library before using any function.
  ML_LIBUSB_INIT_FAILED,///< libusb-1.0 failed to initialize.
  ML_INVALID_POLL_RATE,///< An invalid poll rate was specified, try a value between 1 and 120 or 0 for default (2).
  ML_FAILED_POLL_START,///< Polling failed to start.
  ML_NULL_LAUNCHER,///< A launcher was expected, but a null pointer was found.
  ML_POSITION_WAS_NOT_NULL,///< The specified position was not null when a null position was expected.
  ML_POSITION_WAS_NULL,///< The specified position was null when a valid pointer was expected.
  ML_INDEX_OUT_OF_BOUNDS,///< An array went past the end .
  ML_COUNT_ZERO,///< No items are in the array so it would be impossible to remove one.
  ML_NOT_FOUND,///< An item was not found.
  ML_ARRAY_NOT_NULL,///< A null pointer was expected, but one with possible data was encountered.
  ML_ALLOC_FAILED,///< An allocate failed. Having memory issues?
  ML_LAUNCHER_ARRAY_INCONSISTENT,///< The launcher array is in an inconsisten state. This is the library's fault. Please file a bug report.
  ML_NULL_POINTER,///< A requred pointer was null.
  ML_NO_LAUNCHERS,///< No launchers were detected.
  ML_LAUNCHER_OPEN///< Launcher already open.
} ml_error_codes;


// ********** API Functions **********
// Library init
int16_t ml_init_library();
int16_t ml_cleanup_library();
uint8_t ml_is_library_init();
// Launcher arrays
int16_t ml_get_launcher_array(ml_launcher_t ** *, uint32_t *);
int16_t ml_free_launcher_array(ml_launcher_t **);
// Launcher refrence
int16_t ml_reference_launcher(ml_launcher_t *);
int16_t ml_dereference_launcher(ml_launcher_t *);
// Launcher control
ml_launcher_type ml_get_launcher_type(ml_launcher_t *);
int16_t ml_fire_launcher(ml_launcher_t *);
int16_t ml_move_launcher(ml_launcher_t *, ml_launcher_direction);
int16_t ml_stop_launcher(ml_launcher_t *);
int16_t ml_move_launcher_mseconds(ml_launcher_t *,
                                  ml_launcher_direction, uint32_t);
int16_t ml_zero_launcher(ml_launcher_t *);
int16_t ml_led_on(ml_launcher_t *);
int16_t ml_led_off(ml_launcher_t *);
uint8_t ml_get_led_state(ml_launcher_t *);

#ifdef __cplusplus
}
#endif

#endif
