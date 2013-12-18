/**
 * @file thunder-launcher.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-15
 */

#include "tl_detection.h"



int16_t initialize_library() {
  
  if(main_launch_control.control_initialized) {
    TRACE("Controller already initialized\n");
    return TL_OK;
  }

  main_launch_control.control_initialized = 1;

  main_launch_control.launcher_array = calloc(sizeof(thunder_launcher) * INITIAL_ARRAY_SIZE);


  return TL_OK;
}

int16_t cleanup_library() {

  return TL_NOT_IMPLEMENTED;
}


int16_t poll_for_launcher() {

  return TL_NOT_IMPLEMENTED; 
}

