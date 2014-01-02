/**
 * @file ml_launcher_def.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-02
 */

#ifndef ML_LAUNCHER_DEF_H
#define ML_LAUNCHER_DEF_H

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

#endif
