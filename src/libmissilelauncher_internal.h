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
#include "tlib_debug.h"

// Cross platform compatibility
#if defined(WIN32)
#define ml_second_sleep(seconds) Sleep(1000 * seconds) // Seconds to milliseconds
#else
#include <unistd.h>
#define ml_second_sleep(seconds) sleep(seconds)  // No conversion necessary
#endif

#define ML_MAX_LAUNCHER_ARRAY_SIZE 256
#define ML_INITIAL_LAUNCHER_ARRAY_SIZE 8

typedef struct ml_launcher_t {
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;
  uint8_t device_connected;
  uint32_t ref_count;


  uint32_t horizontal_position;
  uint32_t vertical_position;
  uint32_t led_status;

  libusb_device *usb_device;

  pthread_mutex_t main_lock;
} ml_arr_launcher_t;

struct ml_controller_t {
  int16_t  launcher_count;
  int16_t  launcher_array_size;
  uint8_t   poll_rate_seconds;
  uint8_t   control_initialized;
  uint8_t   currently_polling;

  struct ml_launcher_t **launchers;
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


// ********** Library Functions **********
int16_t _ml_init_controller(ml_controller_t *);
int16_t _ml_cleanup_controller(ml_controller_t *);

int16_t _ml_init_launcher(ml_launcher_t *, libusb_device *);
int16_t _ml_cleanup_launcher(ml_launcher_t **);

int16_t _ml_remove_launcher(ml_launcher_t *);
int16_t _ml_remove_launcher_index(int16_t);
int16_t _ml_add_launcher(ml_launcher_t *);
int16_t _ml_add_launcher_index(ml_launcher_t *, int16_t);

void *_ml_poll_for_launchers(void *);

uint8_t _ml_catagorize_device(struct libusb_device_descriptor *);
int16_t _ml_update_launchers(struct libusb_device **, int);
#endif
