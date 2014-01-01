/**
 * @file ml_launcher.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2013-12-31
 */

#ifndef ML_LAUNCHER_H
#define ML_LAUNCHER_H

#include "ml_defs.h"

#define ML_STD_VENDOR_ID 8483
#define ML_STD_PRODUCT_ID 4112
#define ML_INITIAL_LAUNCHER_ARRAY_SIZE 10

typedef enum ml_launcher_type {
  ML_NOT_LAUNCHER,
  ML_STANDARD_LAUNCHER
} ml_launcher_type;

typedef struct ml_launcher_t {
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;
} ml_launcher_t;

typedef struct ml_arr_launcher_t {
  // Public Variables
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;

  // Private Variables  
  uint32_t horizontal_position;
  uint32_t vertical_position;
  uint32_t led_status;
} ml_arr_launcher_t;

#endif

