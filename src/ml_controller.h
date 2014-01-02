/**
 * @file ml_controller.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2013-12-31
 */

#ifndef ML_CONTROLLER_H
#define ML_CONTROLLER_H

#include "ml_launcher.h"
#include "ml_defs.h"

#define ML_MAX_ATTACHED_DEVICES 256
#define ML_DEFAULT_CONTROL_POLL_RATE 2

typedef void(*ml_void_event_handler)();

typedef struct ml_controller_t {
  uint32_t  launcher_count;
  uint32_t  launcher_array_size;
  uint8_t   poll_rate_seconds;
  uint8_t   control_initialized;
  uint8_t   poll_usb;
  ml_arr_launcher_t **launchers;
  // Event Handlers
  ml_void_event_handler on_launchers_updated;
  //  Mutexes and Locks
  pthread_rwlock_t poll_rate_lock;
  pthread_rwlock_t launcher_array_lock;
  pthread_mutex_t poll_control_mutex;
  // The polling thread
  pthread_t poll_thread;
} ml_controller_t;

static ml_controller_t __attribute__ ((unused)) *ml_main_controller = NULL;
static pthread_mutex_t __attribute__ ((unused)) ml_main_controller_mutex = PTHREAD_MUTEX_INITIALIZER;

int16_t ml_init_library();
int16_t ml_cleanup_library();

int16_t ml_start_continuous_poll();
int16_t ml_stop_continuous_poll();

uint8_t ml_get_poll_rate();
int16_t ml_set_poll_rate(uint8_t poll_rate_seconds);


int16_t _ml_init_controller(ml_controller_t *controller);
int16_t _ml_cleanup_controller(ml_controller_t *controller);

void *_ml_poll_for_launchers(void *target_arg);

#endif
