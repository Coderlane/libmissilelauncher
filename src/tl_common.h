/**
 * @file tl_common.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */


#ifndef TL_COMMON_H
#define TL_COMMON_H


// Cross platform compatibility
#if defined(WIN32)
#define second_sleep(seconds) Sleep(1000 * seconds) // Seconds to milliseconds
#else
#include <unistd.h>
#define second_sleep(seconds) sleep(seconds)  // No conversion necessary
#endif


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <pthread.h>


#include "tl_errors.h"
#include "tlib_debug.h"

#define TL_INITIAL_LAUNCHER_ARRAY_SIZE 10
#define TL_MAX_ATTACHED_DEVICES 256
#define TL_DEFAULT_CONTROL_POLL_RATE 2

typedef enum launcher_status {
  TLS_FREE,
  TLS_CONNECTED,
} launcher_status;

typedef struct thunder_launcher {
  uint8_t           led_status;
  uint32_t          horizontal_position;
  uint32_t          vertical_position;
  launcher_status   current_status;
} thunder_launcher;

typedef struct launch_control {
  uint32_t launcher_count;
  uint32_t launcher_arr_size;
  uint8_t  _poll_rate_seconds;
  uint8_t  control_initialized;
  uint8_t  poll_usb;

  thunder_launcher *launcher_array;

  pthread_rwlock_t poll_rate_lock;
  pthread_mutex_t poll_control_mutex;
  pthread_t poll_thread;
} launch_control;

static launch_control __attribute__ ((unused)) *main_launch_control = NULL;
#endif
