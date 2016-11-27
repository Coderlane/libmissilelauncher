/**
 * @file ml_library.c
 * @brief
 * @author Travis Lane
 * @version 0.5.0
 * @date 2016-11-27
 */

#include <stdint.h>
#include <stdlib.h>

#include "libmissilelauncher.h"
#include "libmissilelauncher_internal.h"

const char *ml_launcher_type_strs[] = {
  "invalid",
  "standard",
  NULL
};

const char *ml_launcher_direction_strs[] = {
  "down",
  "up",
  "left",
  "right",
  NULL
};

const char *ml_error_code_strs[] = {
  "ok",
  "not implemented",
  "test error",
  "unclaimed launcher",
  "library already intialized",
  "library not initialized",
  "libusb error",
  "invalid poll rate",
  "poll start failed",
  "array overflow",
  "array underflow",
  "not found",
  "out of memory",
  "inconsistent array",
  "null pointer",
  "not null pointer",
  "no launchers",
  "launcher already open",
  NULL,
};

/**
 * @brief Initializes the library.
 * This function must be called before all others.
 *
 * @return A status code.
 */
ml_error_code
ml_library_init()
{
  int init_result;
  int16_t failed = 0;


  if (ml_main_controller != NULL) {
    return ML_LIBRARY_ALREADY_INIT;
  }

  // Allocate space for the main controller, calloc so everything is null.
  ml_main_controller = calloc(sizeof(ml_controller_t), 1);
  if (ml_main_controller == NULL) {
    return ML_ALLOC_FAILED;
  }

  // Initialize libusb
  init_result = libusb_init(NULL);
  if (init_result < 0) {
    free(ml_main_controller);
    ml_main_controller = NULL;
    return ML_LIBUSB_ERROR;
  }

  // Initialize the main controller
  failed = _ml_controller_init(ml_main_controller);
  // Check result
  if (failed != ML_OK) {
    // Setup failed
    if (ml_main_controller->launchers != NULL) {
      free(ml_main_controller->launchers);
    }
    free(ml_main_controller);
    ml_main_controller = NULL;
  }

  return failed;
}

/**
 * @brief Cleans up the library when you are done.
 *
 * @return A status code.
 */
ml_error_code
ml_library_cleanup()
{
  int16_t failed = 0;
  if (ml_main_controller == NULL) {
    return ML_LIBRARY_NOT_INIT;
  }
  // Cleanup the controller
  failed = _ml_controller_cleanup(ml_main_controller);
  // Free everything
  free(ml_main_controller);
  ml_main_controller = NULL;
  // Cleanup libusb
  libusb_exit(NULL);
  return failed;
}

/**
 * @brief Checks to see if the library is currently initialized.
 *
 * @return 0 - false 1 - true
 */
uint8_t
ml_library_is_init()
{
  uint8_t result = 0;
  if (ml_main_controller == NULL ||
      ml_main_controller->control_initialized == 0) {
    result = 0;
  } else {
    result = 1;
  }
  return result;
}


/**
 * @brief Convert an error code to its string.
 *
 * @param ec The error code.
 *
 * @return The string for the error code.
 */
const char *
ml_error_to_str(ml_error_code ec)
{
  if (ec >= ML_OK && ec < ML_ERROR_END) {
    return ml_error_code_strs[ec];
  } else {
    return "invalid error code";
  }
}
