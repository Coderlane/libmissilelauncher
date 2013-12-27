/**
 * @file tl_detection.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */

#include "tl_detection.h"

/**
 * @brief Initializes the library for usage.
 *
 * @return 
 */
int16_t initialize_library() {
  int16_t failed = 0;
  if(main_launch_control != NULL) {
    TRACE("Main launch control was not null, possibly already initialized\n");
    return TL_LIBRARY_ALREADY_INIT;
  }
  main_launch_control = calloc(sizeof(launch_control), 1);
  if(main_launch_control == NULL) {
    TRACE("Could not allocate memory for a new launch control\n");
    return TL_CONTROL_ALLOC_FAIL;
  }
  failed = _initialize_control(main_launch_control);
  if(failed) {
    if(main_launch_control->launcher_array != NULL) {
      free(main_launch_control->launcher_array);
    }
    free(main_launch_control);
    main_launch_control = NULL;
  }
  return failed;
}

/**
 * @brief Do not use. Initializes a launch controller.
 *
 * @param init_control The launch controller to initialize
 *
 * @return 
 */
int16_t _initialize_control(launch_control *init_control) {
  if(init_control->control_initialized) {
    TRACE("Controller already initialized\n");
    return TL_CONTROL_ALREADY_INIT;
  }
  init_control->launcher_array = calloc(sizeof(thunder_launcher), INITIAL_ARRAY_SIZE);
  if(init_control->launcher_array == NULL) {
    TRACE("Failed to initialize library. Launcher array was null.\n");
    return TL_CONTROL_ARR_ALLOC_FAIL;
  }

  init_control->launcher_arr_size = INITIAL_ARRAY_SIZE;
  init_control->launcher_count = 0;

  init_control->control_initialized = 1;
  pthread_mutex_init(&(init_control->poll_control_mutex), NULL);
  return TL_OK;
}

/**
 * @brief Resets the library to it's initial state. Ready for re-initialization
 *
 * @return 
 */
int16_t cleanup_library() {
  int16_t failed = 0;
  if(main_launch_control == NULL) {
    TRACE("Could not clean up library. Control was already null.\n");
    return TL_CONTROL_ALREADY_NULL;
  }
  failed = _cleanup_control(main_launch_control);
  if(!failed) {
    free(main_launch_control);
    main_launch_control = NULL;
  }
  return failed;
 }

/**
 * @brief Do not use. Frees all memory related to a launch controller.
 *
 * @param cleanup_control
 *
 * @return 
 */
int16_t _cleanup_control(launch_control *cleanup_control) {
  if(cleanup_control == NULL) {
    TRACE("Could not clean up, control was null.\n");
    return TL_CONTROL_FREE_NULL;
  }
  if(cleanup_control->launcher_array == NULL) {
    TRACE("Could not clean up, launcher array was null.\n");
    return TL_CONTROL_ARR_NULL;
  }
  free(cleanup_control->launcher_array);
  cleanup_control->launcher_arr_size = 0;
  cleanup_control->launcher_count = 0;
  cleanup_control->control_initialized = 0;
  return TL_OK;
}

/**
 * @brief 
 *
 * @param target_control
 *
 * @return 
 */
void *_poll_control_for_launcher(void *target_arg) {
  launch_control *target_control = target_arg;

  if(target_control == NULL) {
    TRACE("Target control was null.\n");
    return NULL;
  }
  pthread_cleanup_push(_poll_control_for_launcher_cleanup, NULL);
  //pthread_cleanup_push(_poll_control_for_launcher_cleanup, NULL);

  for(;;) {
    // The cancellation point 
    pthread_testcancel();

    // Search for new launchers
    
    sleep(TL_CONTROL_POLL_RATE);
  }

  pthread_cleanup_pop(NULL);
  return NULL;
}


void _poll_control_for_launcher_cleanup(void *target_arg) {



}

/**
 * @brief 
 *
 * @return 
 */
int16_t start_continuous_poll() {
  int16_t failed = 0;
  if(main_launch_control == NULL) {
    TRACE("Could not start continuous poll, control was null.\n");
    return TL_CONTROL_WAS_NULL;
  }
  failed = _start_continuous_control_poll(main_launch_control);
  return failed;
}

/**
 * @brief 
 *
 * @return 
 */
int16_t stop_continuous_poll() {
  int16_t failed = 0;
  if(main_launch_control == NULL) {
    TRACE("Could not stop continuous poll, control was null.\n");
    return TL_CONTROL_WAS_NULL;
  }
  failed = _stop_continuous_control_poll(main_launch_control);
  return failed;
}

/**
 * @brief 
 *
 * @param target_control
 *
 * @return 
 */
int16_t _start_continuous_control_poll(launch_control *target_control) {
  int thread_code = 0;
  if(target_control == NULL) {
    TRACE("Could not start continuous poll, control was null.\n");
    return TL_CONTROL_WAS_NULL;
  }

  pthread_mutex_lock(&(target_control->poll_control_mutex));
  target_control->poll_usb = 1;
  thread_code = pthread_create(&(target_control->poll_thread), NULL, _poll_control_for_launcher, (void *) target_control);

  pthread_mutex_unlock(&(target_control->poll_control_mutex));
  return TL_NOT_IMPLEMENTED; 
}

/**
 * @brief 
 *
 * @param target_control
 *
 * @return 
 */
int16_t _stop_continuous_control_poll(launch_control *target_control) {
  
  if(target_control == NULL) {
    TRACE("Could not stop continuous poll, control was null.\n");
    return TL_CONTROL_WAS_NULL;
  }
  
  pthread_mutex_lock(&(target_control->poll_control_mutex));
  target_control->poll_usb = 0;
  
  pthread_cancel(target_control->poll_thread);

  #ifndef NDEBUG
  pthread_join(target_control->poll_thread, NULL)
  #endif

  pthread_mutex_unlock(&(target_control->poll_control_mutex));
  return TL_NOT_IMPLEMENTED; 
}

