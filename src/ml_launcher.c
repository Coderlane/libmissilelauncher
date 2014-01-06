/**
 * @file ml_launcher.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-02
 */

#include "libmissilelauncher_internal.h"

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t _ml_init_launcher(ml_launcher_t *launcher, libusb_device *device) {
  struct libusb_device_descriptor desc;
  if(launcher == NULL) {
    TRACE("Launcher was null. _ml_init_launcher\n");
    return ML_NULL_LAUNCHER;
  }
  libusb_get_device_descriptor(device, &desc);
  launcher->type = _ml_catagorize_device(&desc);
  
  launcher->usb_device = device;
  launcher->ref_count = 0;
  launcher->device_connected = 1;
  libusb_open(device, &(launcher->usb_handle));
#ifdef LINUX
  libusb_detach_kernel_driver(launcher->usb_handle, 0);
  libusb_claim_interface(launcher->usb_handle, 0);
#endif
  pthread_mutex_init(&(launcher->main_lock), NULL);
  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t _ml_cleanup_launcher(ml_launcher_t **launcher) {
  if((*launcher) == NULL) {
    TRACE("Launcher was null. _ml_cleanup_launcher\n");
    return ML_NULL_LAUNCHER;
  }

#ifdef LINUX
  libusb_release_interface((*launcher)->usb_handle, 0);
#endif
  libusb_close((*launcher)->usb_handle);
  pthread_mutex_destroy(&((*launcher)->main_lock));
  free((*launcher));
  launcher = NULL;
  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t ml_reference_launcher(ml_launcher_t *launcher) {
  if(launcher == NULL) {
    TRACE("Launcher was null. ml_refrence_launcher\n");
    return ML_NULL_LAUNCHER;
  }
  pthread_mutex_lock(&(launcher->main_lock));
  launcher->ref_count += 1; 
  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t ml_dereference_launcher(ml_launcher_t *launcher) {
  if(launcher == NULL) {
    TRACE("Launcher was null. ml_derefrence_launcher\n");
    return ML_NULL_LAUNCHER;
  }
  pthread_mutex_lock(&(launcher->main_lock));
  launcher->ref_count -= 1;
  if(launcher->ref_count == 0 && launcher->device_connected == 0) {
    // Not connected and not refrenced
    _ml_remove_launcher(launcher);
    _ml_cleanup_launcher(&launcher);
  }
  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}


/**
 * @brief Fires a missile from the launcher
 *
 * @param launcher The launcher to fire from
 *
 * @return A status code
 */
int16_t ml_fire_launcher(ml_launcher_t *launcher) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  //TODO implement error checking
  _ml_send_command_unsafe(launcher, ML_FIRE_CMD);

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}

/**
 * @brief Moves the launcher in the specified direction, x degrees
 *
 * @param launcher The launcher to move
 * @param direction The specified direction
 * @param degrees The number of degrees to move the launcher.
 *
 * @return A status code
 */
int16_t ml_move_launcher(ml_launcher_t *launcher,
    ml_launcher_direction direction, uint16_t degrees) {

  ml_time_t time;

  pthread_mutex_lock(&(launcher->main_lock));
  
  _ml_degrees_to_time(degrees, &time);
  _ml_move_launcher_unsafe(launcher, direction, &time);

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_NOT_IMPLEMENTED;
}

/**
 * @brief Resets the launcher to 0 degrees of elevation and then centers the launcher
 *
 * @param launcher The launcher to zero
 *
 * @return A status code
 */
int16_t ml_zero_launcher(ml_launcher_t *launcher) {
  ml_time_t left_time, down_time, right_time, up_time;
  
  TRACE("Starting to zero\n");

  pthread_mutex_lock(&(launcher->main_lock));

  switch(launcher->type) {
    case ML_STANDARD_LAUNCHER:
      // Setup movement constants
      _ml_mseconds_to_time(6000, &left_time); // Correct
      _ml_mseconds_to_time(2000, &down_time); // Correct
      _ml_mseconds_to_time(2750, &right_time);
      _ml_mseconds_to_time(100, &up_time); // Correct
      break;
    default:
      TRACE("Unknown type.\n");
      return ML_NOT_IMPLEMENTED;
  }

  TRACE("zeroing\n");

  // Move to known position then to 0 deg vert and center
  _ml_move_launcher_unsafe(launcher, ML_LEFT, &left_time);
  _ml_move_launcher_unsafe(launcher, ML_DOWN, &down_time);
  _ml_move_launcher_unsafe(launcher, ML_RIGHT, &right_time);
  _ml_move_launcher_unsafe(launcher, ML_UP, &up_time);

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t ml_led_on(ml_launcher_t *launcher) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  //TODO implement error checking
  _ml_send_command_unsafe(launcher, ML_LED_ON_CMD);
  launcher->led_status = 1;

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 *
 * @return 
 */
int16_t ml_led_off(ml_launcher_t *launcher) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  //TODO implement error checking
  _ml_send_command_unsafe(launcher, ML_LED_OFF_CMD);
  launcher->led_status = 0;

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_OK;
}

/**
 * @brief Determines if the LED is on or off
 *
 * @param launcher The launcher to check
 *
 * @return 1 = on 0 = off
 */
uint8_t ml_get_led_stat(ml_launcher_t *launcher) {
  uint8_t status = 0;
  pthread_mutex_lock(&(launcher->main_lock));
  
  status = launcher->led_status; 
  
  pthread_mutex_unlock(&(launcher->main_lock));
  return status;
}

/**
 * @brief 
 *
 * @param launcher
 * @param direction
 * @param meseconds
 *
 * @return 
 */
int16_t ml_move_launcher_mseconds(ml_launcher_t *launcher,
    ml_launcher_direction direction, uint32_t mseconds) {
  
  ml_time_t time;
  pthread_mutex_lock(&(launcher->main_lock));
  
  _ml_mseconds_to_time(mseconds, &time);
  _ml_move_launcher_unsafe(launcher, direction, &time);
  
  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_NOT_IMPLEMENTED;
}

/**
 * @brief 
 *
 * @param launcher
 * @param time
 *
 * @return 
 */
int16_t _ml_move_launcher_unsafe(ml_launcher_t *launcher,
         ml_launcher_direction direction, ml_time_t *time) {
 
  // Start movement 
  _ml_send_command_unsafe(launcher, (ml_launcher_cmd) direction);
  // Sleep the set amount of time
  ml_second_sleep(time->seconds);
  ml_msecond_sleep(time->mseconds);
  // Stop movement
  _ml_send_command_unsafe(launcher, ML_STOP_CMD);

  return ML_OK;
}

/**
 * @brief 
 *
 * @param launcher
 * @param cmd
 *
 * @return 
 */
int16_t _ml_send_command_unsafe(ml_launcher_t *launcher, ml_launcher_cmd cmd) {
  uint8_t request_type = 0, request_field = 0;
  uint16_t wValue = 0, wIndex = 0;

  switch(launcher->type) {
    case ML_STANDARD_LAUNCHER:
      request_type = ML_REQUEST_TYPE_SEND;
      request_field = ML_REQUEST_FIELD_SEND;
      wValue = 0;
      wIndex = 0; 
      break;
    default:
      return ML_NOT_IMPLEMENTED;
  }

  libusb_control_transfer(launcher->usb_handle, request_type, request_field,
      wValue, wIndex, ml_cmd_arr[cmd], ML_CMD_ARR_SIZE, 0);

  return ML_OK;
}

/**
 * @brief 
 *
 * @param mseconds
 * @param time
 *
 * @return 
 */
int16_t _ml_mseconds_to_time(uint32_t mseconds, ml_time_t *time) {
  time->seconds = (mseconds / 1000); // Get the seconds
  time->mseconds = mseconds - (time->seconds * 1000); // Get leftovers
  return ML_OK;
}

/**
 * @brief 
 *
 * @param degrees
 * @param time
 *
 * @return 
 */
int16_t _ml_degrees_to_time(uint16_t degrees, ml_time_t *time) {
  time->mseconds = 0;
  time->seconds = 0;
  return ML_NOT_IMPLEMENTED;
}

/*
int16_t _ml_start_launcher_tread(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}

int16_t _ml_stop_launcher_tread(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}

void *_ml_launcher_thread_task(void *launcher_arg) {

  return NULL;
}
*/
