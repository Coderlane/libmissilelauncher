

#ifndef TL_COMMON_H
#define TL_COMMON_H

#include <stdint.h>
#include "tl_errors.h"

#define INITIAL_ARRAY_SIZE 10

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


typedef enum launcher_status {
  TLS_FREE,
  TLS_CONNECTED,

} launcher_status;

static launch_control __attribute__ ((unused)) main_launch_control;

#endif
