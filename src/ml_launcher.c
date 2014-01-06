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
  if(launcher == NULL) {
    TRACE("Launcher was null. _ml_init_launcher\n");
    return ML_NULL_LAUNCHER;
  }
  launcher->usb_device = device;
  launcher->ref_count = 0;
  launcher->device_connected = 1;
  libusb_open(device, &(launcher->usb_handle));
  //TODO This is only used on linux systems, not darwin or windows.
  libusb_detach_kernel_driver(launcher->usb_handle, 0);
  libusb_claim_interface(launcher->usb_handle, 0);
  //
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

  //TODO This is only used on linux systems, not darwin or windows.
  libusb_release_interface((*launcher)->usb_handle, 0);
  // 
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



int16_t _ml_start_launcher_tread(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}

int16_t _ml_stop_launcher_tread(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}

void *_ml_launcher_thread_task(void *launcher_arg) {

  return NULL;
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

  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_fire_cmd, ML_CMD_ARR_SIZE, 0);

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
int16_t ml_move_launcher(ml_launcher_t *launcher, ml_launcher_direction direction, uint16_t degrees) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  //TODO implemente me

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
  pthread_mutex_lock(&(launcher->main_lock));
 
  //TODO implement error checking

  // Move left all the way 
  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_left_cmd, ML_CMD_ARR_SIZE, 0);
  ml_second_sleep(6);
  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_stop_cmd, ML_CMD_ARR_SIZE, 0);
  
  // Move down all the way
  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_down_cmd, ML_CMD_ARR_SIZE, 0);
  ml_second_sleep(2);
  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_stop_cmd, ML_CMD_ARR_SIZE, 0);

  //TODO move to 0 deg elevation and then center 

  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_NOT_IMPLEMENTED;
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

  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
     ML_REQUEST_FIELD_SEND, 0, 0, ml_led_on_cmd, ML_CMD_ARR_SIZE, 0); 
  pthread_mutex_unlock(&(launcher->main_lock));

  launcher->led_status = 1;

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

  libusb_control_transfer(launcher->usb_handle, ML_REQUEST_TYPE_SEND, 
      ML_REQUEST_FIELD_SEND, 0, 0, ml_led_off_cmd, ML_CMD_ARR_SIZE, 0);
  
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
