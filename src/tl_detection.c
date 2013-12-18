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


int16_t poll_for_launcher() {

  return TL_NOT_IMPLEMENTED; 
}

