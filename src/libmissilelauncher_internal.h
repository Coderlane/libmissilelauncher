/**
 * @file libmissilelauncher_internal.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.3
 * @date 2014-01-02
 */

#ifndef LIBMISSILELAUNCHER_INTERNAL_H
#define LIBMISSILELAUNCHER_INTERNAL_H

#include "libmissilelauncher.h"

struct ml_launcher_t {
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;
};

struct ml_arr_launcher_t {
  // Public Variables
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;

  // Private Variables  
  uint32_t horizontal_position;
  uint32_t vertical_position;
  uint32_t led_status;
};

struct ml_controller_t {
  uint32_t  launcher_count;
  uint32_t  launcher_array_size;
  uint8_t   poll_rate_seconds;
  uint8_t   control_initialized;
  uint8_t   currently_polling;
  struct ml_arr_launcher_t **launchers;
  // Event Handlers
  ml_void_event_handler on_launchers_updated;
  //  Mutexes and Locks
  pthread_rwlock_t poll_rate_lock;
  pthread_rwlock_t launcher_array_lock;
  pthread_mutex_t poll_control_mutex;
  // The polling thread
  pthread_t poll_thread;
};

// ***** Global Statics *****
static ml_controller_t __attribute__ ((unused)) *ml_main_controller = NULL;
static pthread_mutex_t __attribute__ ((unused)) ml_main_controller_mutex = PTHREAD_MUTEX_INITIALIZER;

#endif

