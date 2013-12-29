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
  int init_result;
  int16_t failed = 0;
  if(main_launch_control != NULL) {
    TRACE("Main launch control was not null, possibly already initialized\n");
    return TL_LIBRARY_ALREADY_INIT;
  }
  // Allocate space for the main controller, calloc so everything is null.
  main_launch_control = calloc(sizeof(launch_control), 1);
  if(main_launch_control == NULL) {
    TRACE("Could not allocate memory for a new launch control\n");
    return TL_CONTROL_ALLOC_FAIL;
  }
  // Initialize libusb 
  init_result = libusb_init(NULL);
  if(init_result < 0) {
    TRACE("libusb failed with code: %d\n", init_result);
    free(main_launch_control);
    main_launch_control = NULL;
    return TL_LIBUSB_INIT_FAILED;
  }
  // Initialize the main controller
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
  // Setup the array
  init_control->launcher_array = calloc(sizeof(thunder_launcher), TL_INITIAL_LAUNCHER_ARRAY_SIZE);
  if(init_control->launcher_array == NULL) {
    TRACE("Failed to initialize library. Launcher array was null.\n");
    return TL_CONTROL_ARR_ALLOC_FAIL;
  }
  // Set default variables
  init_control->_poll_rate_seconds = TL_DEFAULT_CONTROL_POLL_RATE;
  init_control->launcher_arr_size = TL_INITIAL_LAUNCHER_ARRAY_SIZE;
  init_control->launcher_count = 0;
  // Initialize mutexes and locks
  pthread_rwlock_init(&(init_control->poll_rate_lock), NULL);
  pthread_mutex_init(&(init_control->poll_control_mutex), NULL);
  // Good to go!
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
  libusb_exit(NULL);
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
  cleanup_control->launcher_array = NULL;
  cleanup_control->launcher_arr_size = 0;
  cleanup_control->launcher_count = 0;
  cleanup_control->control_initialized = 0;
  
  // Cleanup mutexes
  pthread_mutex_destroy(&(cleanup_control->poll_control_mutex));
  pthread_rwlock_destroy(&(cleanup_control->poll_rate_lock));
  return TL_OK;
}

int16_t set_poll_rate(uint8_t new_rate) {
  return _set_control_poll_rate(main_launch_control, new_rate);
}

uint8_t get_poll_rate() {
  return _get_control_poll_rate(main_launch_control);
}

int16_t _set_control_poll_rate(launch_control *target_control, uint8_t new_rate) {
  if(target_control == NULL) {
    return TL_CONTROL_WAS_NULL;
  }
  pthread_rwlock_wrlock(&(target_control->poll_rate_lock));
  target_control->_poll_rate_seconds = new_rate;
  pthread_rwlock_unlock(&(target_control->poll_rate_lock));
  return TL_OK;
}

uint8_t _get_control_poll_rate(launch_control *target_control) {
  uint8_t poll_rate = 0;
  if(target_control == NULL) {
    return 0;
  }
  pthread_rwlock_rdlock(&(target_control->poll_rate_lock));
  poll_rate = target_control->_poll_rate_seconds;
  pthread_rwlock_unlock(&(target_control->poll_rate_lock));
  return poll_rate;
}

/**
 * @brief 
 *
 * @param target_control
 *
 * @return 
 */
void *_poll_control_for_launcher(void *target_arg) {
  int device_count = 0, desc_result = 0;
  uint8_t poll_rate = 0;
  int16_t failed = 0;

  libusb_device **devices = NULL;
  libusb_device *cur_device = NULL;
  launch_control *target_control = target_arg;

  if(target_control == NULL) {
    TRACE("Target control was null.\n");
    return NULL;
  }
  pthread_cleanup_push(_poll_control_for_launcher_cleanup, NULL);
  //pthread_cleanup_push(_poll_control_for_launcher_cleanup, NULL);

  poll_rate = _get_control_poll_rate(target_control);


  for(;;) {
    // The cancellation point 
    pthread_testcancel();
    // Get devices  
    device_count = libusb_get_device_list(NULL, &devices);
    if(device_count > 0){
      // Check for a new device...
      for(int i = 0; (cur_device = devices[i]) != NULL && 
                      i < TL_MAX_ATTACHED_DEVICES; i++){
        struct libusb_device_descriptor descriptor;
        desc_result = libusb_get_device_descriptor(cur_device, &descriptor);

        if(is_device_launcher(&descriptor)){
          failed = _control_mount_launcher(target_control, cur_device);
        }
      }
    } 
    // Free
    libusb_free_device_list(devices, 1);
    // Sleep and update poll rate
    second_sleep(poll_rate);
    poll_rate = _get_control_poll_rate(target_control);
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
  TRACE("starting poll\n");
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
  pthread_join(target_control->poll_thread, NULL);
#endif

    pthread_mutex_unlock(&(target_control->poll_control_mutex));
  return TL_NOT_IMPLEMENTED; 
}


uint8_t is_device_launcher(struct libusb_device_descriptor *desc){
  if(desc->idVendor == TL_STD_VENDOR_ID && desc->idProduct == TL_STD_PRODUCT_ID){
    TRACE("found std launcher\n");
    return TL_STANDARD_LAUNCHER;
  } 
  return TL_NOT_LAUNCHER; // Not implmented, return false.
}

int16_t _control_mount_launcher(launch_control *target_control, struct libusb_device *device){

  return TL_NOT_IMPLEMENTED;
}

