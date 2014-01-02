/**
 * @file ml_controller_def.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-02
 */

#ifndef ML_CONTROLLER_DEF_H
#define ML_CONTROLLER_DEF_H

#include <stdint.h>
#include <pthread.h>



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

#endif
