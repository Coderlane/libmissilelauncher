/**
 * @file ml_controller.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2013-12-31
 */

#include "ml_controller.h"

int16_t ml_init_library(){
  int init_result;
  int16_t failed = 0;
  
  pthread_mutex_lock(&ml_main_controller_mutex);
  if(ml_main_controller != NULL) {
    TRACE("Main launch control was not null, possibly already initialized\n");
    pthread_mutex_unlock(&ml_main_controller_mutex);
    return ML_LIBRARY_ALREADY_INIT;
  }
  // Allocate space for the main controller, calloc so everything is null.
  ml_main_controller = calloc(sizeof(ml_controller_t), 1);
  if(ml_main_controller == NULL) {
    TRACE("Could not allocate memory for a new launch control\n");
    pthread_mutex_unlock(&ml_main_controller_mutex);
    return ML_CONTROL_ALLOC_FAIL;
  }
  // Initialize libusb 
  init_result = libusb_init(NULL);
  if(init_result < 0) {
    TRACE("libusb failed with code: %d\n", init_result);
    free(ml_main_controller);
    ml_main_controller = NULL;
    pthread_mutex_unlock(&ml_main_controller_mutex);
    return ML_LIBUSB_INIT_FAILED;
  }
  // Initialize the main controller
  failed = _ml_init_controller(ml_main_controller);
  if(failed) {
    if(ml_main_controller->launchers != NULL) {
      free(ml_main_controller->launchers);
    }
    free(ml_main_controller);
    ml_main_controller = NULL;
  }
  pthread_mutex_unlock(&ml_main_controller_mutex);
  return failed;
}

int16_t _ml_init_controller(ml_controller_t *controller) {
  if(controller->control_initialized) {
    TRACE("Controller already initialized\n");
    return ML_CONTROL_ALREADY_INIT;
  }
  // Setup the array
  controller->launchers = calloc(sizeof(ml_arr_launcher_t), ML_INITIAL_LAUNCHER_ARRAY_SIZE);
  if(controller->launchers == NULL) {
    TRACE("Failed to initialize library. Launcher array was null.\n");
    return ML_CONTROL_ARR_ALLOC_FAIL;
  }
  // Set default variables
  controller->poll_rate_seconds   = ML_DEFAULT_CONTROL_POLL_RATE;
  controller->launcher_array_size = ML_INITIAL_LAUNCHER_ARRAY_SIZE;
  controller->launcher_count = 0;
  // Initialize mutexes and locks
  pthread_rwlock_init(&(controller->poll_rate_lock), NULL);
  pthread_mutex_init(&(controller->poll_control_mutex), NULL);
  // Good to go!
  controller->control_initialized = 1;
  return ML_OK;
}
