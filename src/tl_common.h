

#ifndef TL_COMMON_H
#define TL_COMMON_H

#include <stdint.h>
#include "tl_errors.h"

typedef struct thunder_launcher {
  uint8_t   led_status;
  uint32_t  horizontal_position;
  uint32_t  vertical_position;
} thunder_launcher;

typedef struct launch_control {
  uint32_t launcher_count;

} launch_control;

#endif
