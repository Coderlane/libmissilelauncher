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

int16_t ml_cleanup_library() { 
  int16_t failed = 0;
  pthread_mutex_lock(&ml_main_controller_mutex);
  if(ml_main_controller == NULL) {
    TRACE("Could not clean up library. Library not initialized.\n");
    return ML_LIBRARY_NOT_INIT;
  }
  failed = _ml_cleanup_controller(ml_main_controller);
  free(ml_main_controller);
  ml_main_controller = NULL;
  libusb_exit(NULL);
  pthread_mutex_unlock(&ml_main_controller_mutex);
  return failed;
}

int16_t _ml_cleanup_controller(ml_controller_t *controller) {
  if(controller == NULL) {
    TRACE("Could not clean up, control was null.\n");
    return ML_CONTROL_FREE_NULL;
  }
  if(controller->launchers == NULL) {
    TRACE("Could not clean up, launcher array was null.\n");
    return ML_CONTROL_ARR_NULL;
  }
  free(controller->launchers);
  controller->launchers = NULL;
  controller->launcher_array_size = 0;
  controller->launcher_count = 0;
  controller->control_initialized = 0;

  // Cleanup mutexes
  pthread_mutex_destroy(&(controller->poll_control_mutex));
  pthread_rwlock_destroy(&(controller->poll_rate_lock));
  return ML_OK;
}

uint8_t ml_is_library_init() {
  if(ml_main_controller == NULL) {
    TRACE("Library not initialized.\n");
    return 0;
  } 
  return 1;
}

int16_t ml_start_continuous_poll() {
  int thread_code = 0;
  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));
  if(ml_main_controller->currently_polling) {
    pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
    return ML_OK;
  }

  thread_code = pthread_create(&(ml_main_controller->poll_thread), NULL, _ml_poll_for_launchers, (void *) ml_main_controller);
  ml_main_controller->currently_polling = 1;
  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
  if(thread_code != 0) {
    return ML_FAILED_POLL_START;
  }
  return ML_OK;
}

int16_t ml_stop_continuous_poll() {
  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));
  if(!(ml_main_controller->currently_polling)) {
    pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
    return ML_OK;
  }
  pthread_cancel(ml_main_controller->poll_thread);

#ifndef NDEBUG
  pthread_join(ml_main_controller->poll_thread, NULL);
#endif
  ml_main_controller->currently_polling = 0;
  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
  return ML_OK;
}


void *_ml_poll_for_launchers(void *target_arg) {
  int device_count = 0, desc_result = 0;
  uint8_t poll_rate = 0;
  int16_t failed = 0;

  libusb_device **devices = NULL;
  libusb_device *cur_device = NULL;

  if(!ml_is_library_init()) return NULL;

  poll_rate = ml_get_poll_rate();

  for(;;) {
    // The cancellation point 
    pthread_testcancel();
    // Get devices  
    device_count = libusb_get_device_list(NULL, &devices);
    

    // Free
    libusb_free_device_list(devices, 1);
    // Sleep and update poll rate
    ml_second_sleep(poll_rate);
    poll_rate = ml_get_poll_rate();
  }
}

uint8_t ml_is_polling() {
  uint8_t polling = 0;

  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));
  polling = ml_main_controller->currently_polling;
  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
  return polling;
}

/**
 * @brief 
 *
 * @return 
 */
uint8_t ml_get_poll_rate() {
  uint8_t poll_rate;

  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_rwlock_rdlock(&(ml_main_controller->poll_rate_lock));
  poll_rate = ml_main_controller->poll_rate_seconds;
  pthread_rwlock_unlock(&(ml_main_controller->poll_rate_lock));
  return poll_rate;
}

/**
 * @brief 
 *
 * @param poll_rate_seconds The new pollrate in seconds equal to 
 * or between ML_MAX_POLL_RATE and ML_MIN_POLL_RATE
 *
 * @return A result code, 0 (OK),  ML_LIBRARY_NOT_INIT, or ML_INVALID_POLL_RATE
 */
int16_t ml_set_poll_rate(uint8_t poll_rate_seconds) {

  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  if(poll_rate_seconds > ML_MAX_POLL_RATE || poll_rate_seconds < ML_MIN_POLL_RATE) {
    return ML_INVALID_POLL_RATE;
  }
  pthread_rwlock_wrlock(&(ml_main_controller->poll_rate_lock));
  ml_main_controller->poll_rate_seconds = poll_rate_seconds;
  pthread_rwlock_unlock(&(ml_main_controller->poll_rate_lock));
  return ML_OK;
}


