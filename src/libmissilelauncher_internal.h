/**
 * @file libmissilelauncher_internal.h
 * @brief Library internal header, I strongly recommend against using these functions. 
 * @author Travis Lane
 * @version 0.0.3
 * @date 2014-01-02
 */

#ifndef LIBMISSILELAUNCHER_INTERNAL_H
#define LIBMISSILELAUNCHER_INTERNAL_H

#include "libmissilelauncher.h"
#include "tlib_debug.h"

// Cross platform compatibility
#if defined(WINDOWS)
/* Use for only second values */
#define ml_second_sleep(seconds) Sleep(1000 * seconds) // Seconds to milliseconds
/* Use for less than one second */
#define ml_msecond_sleep(mseconds) Sleep(mseconds) // milliseconds
#else
#include <unistd.h>
/* Use only for second values */
#define ml_second_sleep(seconds) sleep(seconds)  // No conversion necessary
/* Use for less than one second */
#define ml_msecond_sleep(mseconds) usleep(mseconds * 1000)
#endif

#define ML_MAX_LAUNCHER_ARRAY_SIZE 256
#define ML_INITIAL_LAUNCHER_ARRAY_SIZE 8

#define ML_CMD_ARR_SIZE 2
#define ML_REQUEST_TYPE_SEND 0x21
#define ML_REQUEST_FIELD_SEND 0x09

typedef struct ml_launcher_t {
  ml_launcher_type type;
  uint8_t usb_bus;
  uint8_t usb_device_number;
  uint8_t device_connected;
  uint32_t ref_count;

  uint32_t horizontal_position;
  uint32_t vertical_position;
  uint8_t led_status;

  libusb_device *usb_device;
  libusb_device_handle *usb_handle;

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

typedef struct ml_time_t {
  uint32_t seconds;
  uint32_t mseconds;
} ml_time_t;

// ***** Global Statics *****
static ml_controller_t __attribute__ ((unused)) *ml_main_controller = NULL;
static pthread_mutex_t __attribute__ ((unused)) ml_main_controller_mutex = PTHREAD_MUTEX_INITIALIZER;

// Launcher commands
static unsigned char ml_down_cmd[ML_CMD_ARR_SIZE] =    {0x02, 0x01};
static unsigned char ml_up_cmd[ML_CMD_ARR_SIZE] =      {0x02, 0x02};
static unsigned char ml_left_cmd[ML_CMD_ARR_SIZE] =    {0x02, 0x04};
static unsigned char ml_right_cmd[ML_CMD_ARR_SIZE] =   {0x02, 0x08};
static unsigned char ml_led_on_cmd[ML_CMD_ARR_SIZE] =  {0x03, 0x01};
static unsigned char ml_led_off_cmd[ML_CMD_ARR_SIZE] = {0x03, 0x00};
static unsigned char ml_fire_cmd[ML_CMD_ARR_SIZE] =    {0x02, 0x10};
static unsigned char ml_stop_cmd[ML_CMD_ARR_SIZE] =    {0x02, 0x20};

// Launcher Command Enum
typedef enum ml_launcher_cmd {
  ML_DOWN_CMD,
  ML_UP_CMD,
  ML_LEFT_CMD,
  ML_RIGHT_CMD,
  ML_FIRE_CMD,
  ML_STOP_CMD,
  ML_LED_ON_CMD,
  ML_LED_OFF_CMD,
  ML_COMMAND_COUNT
} ml_launcher_cmd;

// Launcher command array for fast look up
static unsigned char __attribute__ ((unused)) *ml_cmd_arr[ML_COMMAND_COUNT] = {
  ml_down_cmd, ml_up_cmd, ml_left_cmd, ml_right_cmd, ml_fire_cmd,
  ml_stop_cmd, ml_led_on_cmd, ml_led_off_cmd};

// ********** Library Functions **********
// Controller Init
int16_t _ml_init_controller(ml_controller_t *);
int16_t _ml_cleanup_controller(ml_controller_t *);
// Polling
int16_t _ml_start_poll_unsafe();
int16_t _ml_stop_poll_unsafe();
void *_ml_poll_for_launchers(void *);
int16_t _ml_update_launchers(struct libusb_device **, int);
int16_t _ml_get_launchers_from_devices(libusb_device **, int, libusb_device ***, uint32_t *);
int16_t _ml_remove_disconnected_launchers(libusb_device **, uint32_t);
int16_t _ml_add_new_launchers(libusb_device **, uint32_t);
// Launcher Array
int16_t _ml_remove_launcher(ml_launcher_t *);
int16_t _ml_remove_launcher_index(int16_t);
int16_t _ml_add_launcher(ml_launcher_t *);
int16_t _ml_add_launcher_index(ml_launcher_t *, int16_t);
// Launcher Init
int16_t _ml_init_launcher(ml_launcher_t *, libusb_device *);
int16_t _ml_cleanup_launcher(ml_launcher_t **);
uint8_t _ml_catagorize_device(struct libusb_device_descriptor *);
// Launcher Control
int16_t _ml_move_launcher_unsafe(ml_launcher_t *, ml_launcher_direction, ml_time_t *);
int16_t _ml_send_command_unsafe(ml_launcher_t *, ml_launcher_cmd);
// Time Conversions
int16_t _ml_mseconds_to_time(uint32_t, ml_time_t *);
int16_t _ml_degrees_to_time(uint16_t, ml_time_t *); //TODO Take measurements and finish

/*
// TODO Implement these functions
int16_t _ml_start_launcher_tread(ml_launcher_t *);
int16_t _ml_stop_launcher_tread(ml_launcher_t *);
void *_ml_launcher_thread_task(void *);
*/
#endif
