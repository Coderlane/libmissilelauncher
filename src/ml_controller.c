/**
 * @file ml_controller.c
 * @brief Functions releated to the launcher controller 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2013-12-31
 */

#include "libmissilelauncher_internal.h"

/**
 * @brief Initializes the library. 
 * This function must be called before all others.
 *
 * @return A status code
 */
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
    return ML_ALLOC_FAILED;
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

/**
 * @brief initializes a controller
 *
 * @param controller The controller to initialize
 * 
 * @return A status code 
 */
int16_t _ml_init_controller(ml_controller_t *controller) {
  if(controller->control_initialized) {
    TRACE("Controller already initialized\n");
    return ML_CONTROL_ALREADY_INIT;
  }
  // Setup the array
  controller->launchers = calloc(sizeof(ml_arr_launcher_t),
      ML_INITIAL_LAUNCHER_ARRAY_SIZE);

  if(controller->launchers == NULL) {
    TRACE("Failed to initialize library. Launcher array was null.\n");
    return ML_ALLOC_FAILED;
  }
  // Set default variables
  controller->poll_rate_seconds   = ML_DEFAULT_POLL_RATE;
  controller->launcher_array_size = ML_INITIAL_LAUNCHER_ARRAY_SIZE;
  controller->launcher_count = 0;
  // Initialize mutexes and locks
  pthread_rwlock_init(&(controller->poll_rate_lock), NULL);
  pthread_mutex_init(&(controller->poll_control_mutex), NULL);
  // Good to go!
  controller->control_initialized = 1;
  return ML_OK;
}

/**
 * @brief Cleans up the library when you are done
 *
 * @return A status code
 */
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

/**
 * @brief Cleans up the controller when the library is done
 *
 * @param controller The controller to clean up
 *
 * @return A status code
 */
int16_t _ml_cleanup_controller(ml_controller_t *controller) {
  // Error checking
  if(controller == NULL) {
    TRACE("Could not clean up, control was null.\n");
    return ML_CONTROL_FREE_NULL;
  }
  if(controller->launchers == NULL) {
    TRACE("Could not clean up, launcher array was null.\n");
    return ML_CONTROL_ARR_NULL;
  }
  // Clean up array
  free(controller->launchers);
  controller->launchers = NULL;
  controller->launcher_array_size = 0;
  controller->launcher_count = 0;
  // Cleanup mutexes
  pthread_mutex_destroy(&(controller->poll_control_mutex));
  pthread_rwlock_destroy(&(controller->poll_rate_lock));

  controller->control_initialized = 0;
  return ML_OK;
}

/**
 * @brief Checks to see if the library is currently initialized
 *
 * @return 0 - false 1 - true 
 */
uint8_t ml_is_library_init() {
  if(ml_main_controller == NULL) {
    TRACE("Library not initialized.\n");
    return 0;
  } 
  return 1;
}

/**
 * @brief Starts continuous polling
 *
 * @return A status code
 */
int16_t ml_start_continuous_poll() {
  int thread_code = 0;
  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));

  if(ml_main_controller->currently_polling) {
    // If it is already polling we can exit
    pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
    return ML_OK;
  }

  thread_code = pthread_create(&(ml_main_controller->poll_thread), NULL,
      _ml_poll_for_launchers, (void *) ml_main_controller);

  if(thread_code != 0) {
    // Thread failed to start
    TRACE("Thread failed to start. (ml_start_continuous_poll)\n");

    ml_main_controller->currently_polling = 0;
    pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));

    return ML_FAILED_POLL_START;
  }

  ml_main_controller->currently_polling = 1;
  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));

  return ML_OK;
}

/**
 * @brief Cancels the continuous polling
 *
 * @return A status code
 */
int16_t ml_stop_continuous_poll() {
  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));

  if((ml_main_controller->currently_polling) == 0) {
    // Already not polling
    pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
    return ML_OK;
  }

  // Cancel the thread and exit
  pthread_cancel(ml_main_controller->poll_thread);

#ifndef NDEBUG
  // Wait to exit when debugging
  pthread_join(ml_main_controller->poll_thread, NULL);
#endif
  ml_main_controller->currently_polling = 0;

  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
  return ML_OK;
}


/**
 * @brief Thread function to poll for new devices
 *
 * @return NULL on error
 */
void *_ml_poll_for_launchers(void * __attribute__ ((unused)) unused) {
  int device_count = 0;
  uint8_t poll_rate = 0;
  libusb_device **devices = NULL;

  if(!ml_is_library_init()) return NULL;
  TRACE("Polling\n");
  poll_rate = ml_get_poll_rate();

  // This loop won't exit. We just check to see if the 
  for(;;) {
    // The cancellation point 
    pthread_testcancel();
    TRACE("loop\n");
    // Get devices  
    device_count = libusb_get_device_list(NULL, &devices);
    _ml_update_launchers(devices, device_count); 
    //TODO: Scan and add to array if new. Else continue

    // Free
    libusb_free_device_list(devices, 1);
    // Sleep and update poll rate
    ml_second_sleep(poll_rate);
    poll_rate = ml_get_poll_rate();
  }
}

/**
 * @brief Checks to see if the controller is currently polling.
 *
 * @return 0 - false , 1 - true
 */
uint8_t ml_is_polling() {
  uint8_t polling = 0;
  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  pthread_mutex_lock(&(ml_main_controller->poll_control_mutex));

  polling = ml_main_controller->currently_polling;

  pthread_mutex_unlock(&(ml_main_controller->poll_control_mutex));
  return polling;
}

/**
 * @brief Gets the current poll rate
 *
 * @return the current poll rate, or 0 if something went wrong.
 */
uint8_t ml_get_poll_rate() {
  uint8_t poll_rate;
  if(!ml_is_library_init()) return 0; // 0 is an invalid poll rate

  pthread_rwlock_rdlock(&(ml_main_controller->poll_rate_lock));

  poll_rate = ml_main_controller->poll_rate_seconds;

  pthread_rwlock_unlock(&(ml_main_controller->poll_rate_lock));
  return poll_rate;
}

/**
 * @brief Sets a new poll rate for the library to use.
 *
 * @param poll_rate_seconds The new pollrate in seconds equal to 
 * or between ML_MAX_POLL_RATE and ML_MIN_POLL_RATE or
 * 0 to reset to the default poll rate
 *
 * @return A result code, 0 (OK),  ML_LIBRARY_NOT_INIT, or ML_INVALID_POLL_RATE
 */
int16_t ml_set_poll_rate(uint8_t poll_rate_seconds) {

  if(!ml_is_library_init()) return ML_LIBRARY_NOT_INIT;

  if(poll_rate_seconds == 0) {
    pthread_rwlock_wrlock(&(ml_main_controller->poll_rate_lock));
    ml_main_controller->poll_rate_seconds = ML_DEFAULT_POLL_RATE;
    pthread_rwlock_unlock(&(ml_main_controller->poll_rate_lock));
    return ML_OK;
  }

  if(poll_rate_seconds > ML_MAX_POLL_RATE ||
      poll_rate_seconds < ML_MIN_POLL_RATE) {
    return ML_INVALID_POLL_RATE;
  }
  pthread_rwlock_wrlock(&(ml_main_controller->poll_rate_lock));
  ml_main_controller->poll_rate_seconds = poll_rate_seconds;
  pthread_rwlock_unlock(&(ml_main_controller->poll_rate_lock));
  return ML_OK;
}

/**
 * @brief This function catagorizes the type of launcher.
 * For now we only support:
 * Thunder Launchers - ML_STANDARD_LAUNCHER
 *
 * @param desc The USB device descriptro
 *
 * @return They type of launcher or ML_NOT_LAUNCHER if it isn't a launcher
 */
uint8_t _ml_catagorize_device(struct libusb_device_descriptor *desc) {
  if(desc->idProduct == ML_STD_PRODUCT_ID && 
      desc->idVendor == ML_STD_VENDOR_ID) {
    TRACE("found std launcher\n");
    return ML_STANDARD_LAUNCHER;
  }
  return ML_NOT_LAUNCHER;
}

/**
 * @brief Checks the USB device array and sees if any devices were added or removed.
 * If any devices were added or removed we update the array.
 *
 * @param devices The devices libusb found
 * @param device_count the number of devices found
 *
 * @return A status code, ML_OK if everything went well
 */
int16_t _ml_update_launchers(struct libusb_device **devices, int device_count) {
  libusb_device **found_launchers = NULL;
  uint32_t found_launchers_count = 0;

  _ml_get_launchers_from_devices(devices, device_count, &found_launchers,
                                 &found_launchers_count);

  pthread_rwlock_wrlock(&(ml_main_controller->launcher_array_lock));

  _ml_remove_disconnected_launchers(found_launchers, found_launchers_count);

  _ml_add_new_launchers(found_launchers, found_launchers_count);

  pthread_rwlock_unlock(&(ml_main_controller->launcher_array_lock));

  free(found_launchers);
  return ML_NOT_IMPLEMENTED;
}

int16_t _ml_get_launchers_from_devices(libusb_device **devices, int device_count,
    libusb_device ***found_launchers, uint32_t *launchers_found) {

  libusb_device *found_device = NULL;
  uint32_t found_count = 0;

  (*launchers_found) = 0;

  // Make a new array of devices
  for(int i = 0; i < device_count && (found_device = devices[i]) != NULL; i++) {
    struct libusb_device_descriptor device_descriptor;
    libusb_get_device_descriptor(found_device, &device_descriptor);

    // Check if the device is a launcher
    if(_ml_catagorize_device(&device_descriptor) != ML_NOT_LAUNCHER) {
      // Device is launcher
      found_count += 1;

      (*found_launchers) = realloc((*found_launchers), 
          (found_count + 1) * sizeof(libusb_device *));

      if((*found_launchers) == NULL) {
        TRACE("realloc failed. _ml_update_launchers\n");
        return ML_ALLOC_FAILED;
      }

      (*found_launchers)[found_count - 1] = found_device;
      (*found_launchers)[found_count] = NULL;
    } 
  }

  (*launchers_found) = found_count;
  return ML_OK;
}

int16_t _ml_remove_disconnected_launchers(libusb_device **found_launchers,
          uint32_t found_launchers_count) {

  libusb_device *found_device = NULL;
  ml_launcher_t *known_launcher = NULL;

  // Check to see if we need to remove any devices 
  for(uint16_t known_it = 0; known_it < ml_main_controller->launcher_array_size; known_it++){
    // We'll be checking if the known device was not found in the new array of mounted devices. 
    uint8_t found = 0;

    known_launcher = ml_main_controller->launchers[known_it];
    if(known_launcher == NULL) {
      continue;
    }
    pthread_mutex_lock(&(known_launcher->main_lock));
    
    // Check if any devices were found.
    if(found_launchers != NULL) { 
      // Check the devices we found
      for(uint16_t found_it = 0; found_it < found_launchers_count && 
          (found_device = found_launchers[found_it]) != NULL && found == 0; found_it++) {

        if(known_launcher->usb_device == found_device){
          found = 1;
        }
      }
    }
    // Update the connected state
    known_launcher->device_connected = found;

    if(known_launcher->device_connected == 0 &&
        known_launcher->ref_count == 0) {

      // No one is refrencing the device, so we can free it.
      pthread_mutex_unlock(&(known_launcher->main_lock));
      _ml_remove_launcher_index(known_it);
      _ml_cleanup_launcher(&known_launcher);
    } else {
      pthread_mutex_unlock(&(known_launcher->main_lock));
    }
  }

  return ML_OK;
}

int16_t _ml_add_new_launchers(libusb_device **found_launchers,
          uint32_t found_launchers_count) {
  
  libusb_device *found_device = NULL;
  ml_launcher_t *known_launcher = NULL;

  // Check for any new devices
  for(uint16_t found_it = 0; found_it < found_launchers_count &&
      (found_device = found_launchers[found_it]) != NULL; found_it++){
    // We'll be checking if the device is already present
    uint8_t found = 0;

    for(uint16_t known_it = 0; known_it < ml_main_controller->launcher_array_size &&
        found == 0; known_it++) {

      known_launcher = ml_main_controller->launchers[known_it];
      if(known_launcher != NULL && known_launcher->usb_device == found_device) {
        // Found something identical
        found = 1;
      }
    }
    if(found == 0) {
      // Device wasn't found in the array of known devices. Add it.
      ml_launcher_t *new_launcher = calloc(sizeof(ml_launcher_t), 1);
      if(new_launcher == NULL) {
        TRACE("Failed to allocate a new launcher. (_ml_update_launchers)\n");
      } else {
        _ml_init_launcher(new_launcher, found_device);
        _ml_add_launcher(new_launcher);
      }
    }
  }
  return ML_OK;
}

/**
 * @brief Allocates space for and returns a new array of launchers.
 * To clean up use ml_free_launcher_array. Modifying values in the array
 * will produce undexpected results. Treat this array as a constant.
 * NOTE: if you don't want to lose access to a launcher use 
 * ml_reference_launcher and ml_dereference_launcher when you are done.
 *
 * @param new_arr Pointer to where you want the array
 * @param count The number of items found
 *
 * @return A status code, ML_OK if everything went well.
 */
int16_t ml_get_launcher_array(ml_launcher_t ***new_arr, uint32_t *count) {
  ml_launcher_t *cur_launcher;
  int16_t new_index = 0, new_count = 0;
  // Error checking
  if((*new_arr) != NULL) {
    TRACE("Launcher array was not null. (ml_get_launcher_array)\n");
    return ML_ARRAY_NOT_NULL;
  }
  // Lock the launcher array for read
  pthread_rwlock_rdlock(&(ml_main_controller->launcher_array_lock));
  new_count = ml_main_controller->launcher_count;
  (*count) = new_count;
  if(new_count == 0) {
    return ML_NO_LAUNCHERS;
  }
  // Allocate space for the new array.
  (*new_arr) = malloc(sizeof(ml_launcher_t *) * (new_count + 1));
  if((*new_arr) == NULL) {
    TRACE("Launcher array was null after malloc. (ml_get_launcher_array)\n");
    pthread_rwlock_unlock(&(ml_main_controller->launcher_array_lock));
    return ML_ALLOC_FAILED;
  }

  (*new_arr)[new_count] = NULL;

  // Find the launchers
  for(int16_t i = 0; i < ml_main_controller->launcher_array_size; i++) {
    // Found a launcher
    cur_launcher = ml_main_controller->launchers[i];
    if(cur_launcher != NULL) {
      if(new_index >= ml_main_controller->launcher_count) {
        TRACE("Found more launchers than should be present. (ml_get_launcher_array)\n");
        pthread_rwlock_unlock(&(ml_main_controller->launcher_array_lock));
        return ML_LAUNCHER_ARRAY_INCONSISTENT;
      }
      // Refrence the launcher since this will be going back to the programmer
      ml_reference_launcher(cur_launcher);
      (*new_arr)[new_index] = cur_launcher;
      new_index += 1;
    }
  } 

  pthread_rwlock_unlock(&(ml_main_controller->launcher_array_lock));
  return ML_OK;
}

/**
 * @brief Frees the array of launchers ml_get_launcher_array provides.
 * Dereferences every launcher in the array so they can be cleaned up later.
 * NOTE: if you don't want to lose access to a launcher use 
 * ml_reference_launcher and ml_dereference_launcher when you are done.
 *
 * @param free_arr The array to free
 *
 * @return A status code
 */
int16_t ml_free_launcher_array(ml_launcher_t **free_arr) {
  ml_launcher_t *cur_launcher = NULL;
  int16_t index = 0;

  if(free_arr == NULL) {
    TRACE("Could not free array, array was null. (ml_free_launcher_array)\n");
    return ML_ARRAY_WAS_NULL;
  }

  while((cur_launcher = free_arr[index]) != NULL) {
    // Dereference each launcher.
    ml_dereference_launcher(cur_launcher);
    index++;
  }
  // Free the leftover array
  free(free_arr);
  return ML_OK;
}


/**
 * @brief Removes a launcher from the array. Be sure to lock the array for writing first
 *
 * @param launcher The launcher to remove
 *
 * @return A status value, ML_OK if it is okay.
 */
int16_t _ml_remove_launcher(ml_launcher_t * launcher) {
  /* This function is not thread safe, please lock the array first */

  for(int16_t i = 0; i < ml_main_controller->launcher_count; i++) {
    // Search for the launcher of interest
    if(ml_main_controller->launchers[i] == launcher) {
      return _ml_remove_launcher_index(i);
    }
  }
  return ML_NOT_FOUND;
}

/**
 * @brief Removes a launcher from the array. Be sure to lock the array for writing first
 *
 * @param index The index of the item to remove
 *
 * @return A status value, ML_OK if it is okay.
 */
int16_t _ml_remove_launcher_index(int16_t index) {
  /* This function is not thread safe, please lock the array first */

  // Error checking
  if(ml_main_controller->launchers[index] == NULL) {
    TRACE("Addition position was null.\n");
    return ML_POSITION_WAS_NULL;
  }
  if(index >= ml_main_controller->launcher_array_size || index < 0) {
    TRACE("Index out of bounds.\n");
    return ML_INDEX_OUT_OF_BOUNDS;
  }
  if(ml_main_controller->launcher_count <= 0) {
    TRACE("Nothing to remove.\n");
    return ML_COUNT_ZERO;
  }
  // Everything looks good, decrement and set as null. We do not free here.
  ml_main_controller->launcher_count -= 1;
  ml_main_controller->launchers[index] = NULL;
  return ML_OK;
}

/**
 * @brief adds a launcher to the array. Be sure to lock the array for writing first
 *
 * @param launcher The item to add
 *
 * @return A status value, ML_OK if it is okay
 */
int16_t _ml_add_launcher(ml_launcher_t *launcher) {
  /* This function is not thread safe, please lock the array first */

  for(int16_t i = 0; i < ml_main_controller->launcher_array_size; i++) {
    // Find an empty spot
    if(ml_main_controller->launchers[i] == NULL) {
      TRACE("adding launcher to index: %d\n", i);
      return _ml_add_launcher_index(launcher, i);
    }
  }
  // No freespace found, needs a resize.
  // Not yet implemented...
  return ML_NOT_IMPLEMENTED;
}

/**
 * @brief adds a launcher to the array. Be sure to lock the array for writing first
 *
 * @param launcher The launcher to add
 * @param index The index to insert it in
 *
 * @return A status code, ML_OK if it is okay
 */
int16_t _ml_add_launcher_index(ml_launcher_t *launcher, int16_t index) {
  /* This function is not thread safe, please lock the array first */
  if(ml_main_controller->launchers[index] != NULL) {
    TRACE("Addition position was not null.\n");
    return ML_POSITION_WAS_NOT_NULL;
  }
  // Update index and add
  ml_main_controller->launcher_count += 1;
  ml_main_controller->launchers[index] = launcher;
  return ML_NOT_IMPLEMENTED;
}

