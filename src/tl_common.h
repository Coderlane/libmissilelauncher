/**
 * @file tl_common.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */


#ifndef TL_COMMON_H
#define TL_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <libusb.h>


#include "tl_errors.h"
#include "tlib_debug.h"

#define INITIAL_ARRAY_SIZE 10

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
  thunder_launcher *launcher_array;
  uint8_t  control_initialized;
} launch_control;



static launch_control __attribute__ ((unused)) *main_launch_control = NULL;
#endif
