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
  TRACE("detach result %d\n", libusb_detach_kernel_driver(launcher->usb_handle, 0));
  TRACE("claim result %d\n", libusb_claim_interface(launcher->usb_handle, 0));
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
  libusb_release_interface((*launcher)->usb_handle, 0);
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

int16_t ml_file_launcher(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}

int16_t ml_move_launcher(ml_launcher_t *launcher, ml_launcher_direction direction, uint16_t degrees) {

  return ML_NOT_IMPLEMENTED;
}

int16_t ml_zero_launcher(ml_launcher_t *launcher) {

  return ML_NOT_IMPLEMENTED;
}


