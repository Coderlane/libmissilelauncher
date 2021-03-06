/**
 * @file libmissilelauncher_internal.h
 * @brief Library internal header, I strongly recommend against using these functions.
 * @author Travis Lane
 * @version 0.5.0
 * @date 2016-11-27
 */

#ifndef LIBMISSILELAUNCHER_INTERNAL_H
#define LIBMISSILELAUNCHER_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>

#include <libusb-1.0/libusb.h>

#include "libmissilelauncher.h"

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

typedef struct ml_launcher_t
{
	ml_launcher_type type;
	uint8_t   usb_bus;
	uint8_t   usb_device_number;
	uint8_t   device_connected;
	uint32_t  ref_count;
	bool      claimed;

	uint32_t  horizontal_position;
	uint32_t  vertical_position;
	uint8_t   led_status;

	libusb_device *usb_device;
	libusb_device_handle *usb_handle;

	struct ml_controller_t *controller;
} ml_arr_launcher_t;

struct ml_controller_t
{
	int16_t  launcher_count;
	int16_t  launcher_array_size;
	uint8_t  poll_rate_seconds;
	uint8_t  control_initialized;
	uint8_t  currently_polling;

	struct ml_launcher_t **launchers;
};

typedef struct ml_time_t
{
	uint32_t seconds;
	uint32_t mseconds;
} ml_time_t;

// ***** Global Statics *****
static ml_controller_t __attribute__ ((unused)) *ml_main_controller = NULL;

// Launcher commands
static unsigned char ml_down_cmd[ML_CMD_ARR_SIZE] =      {0x02, 0x01};
static unsigned char ml_up_cmd[ML_CMD_ARR_SIZE] =        {0x02, 0x02};
static unsigned char ml_left_cmd[ML_CMD_ARR_SIZE] =      {0x02, 0x04};
static unsigned char ml_right_cmd[ML_CMD_ARR_SIZE] =     {0x02, 0x08};
static unsigned char ml_led_on_cmd[ML_CMD_ARR_SIZE] =    {0x03, 0x01};
static unsigned char ml_led_off_cmd[ML_CMD_ARR_SIZE] =   {0x03, 0x00};
static unsigned char ml_fire_cmd[ML_CMD_ARR_SIZE] =      {0x02, 0x10};
static unsigned char ml_stop_cmd[ML_CMD_ARR_SIZE] =      {0x02, 0x20};

// Launcher Command Enum
typedef enum ml_launcher_cmd
{
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
static unsigned char __attribute__ ((unused)) *ml_cmd_arr[ML_COMMAND_COUNT] =
{
	ml_down_cmd, ml_up_cmd, ml_left_cmd, ml_right_cmd, ml_fire_cmd,
	ml_stop_cmd, ml_led_on_cmd, ml_led_off_cmd
};

// ********** Library Functions **********
#ifdef __cplusplus
extern "C" {
#endif

// Controller Init
ml_error_code _ml_controller_init(ml_controller_t *);
ml_error_code _ml_controller_cleanup(ml_controller_t *);

// Polling
ml_error_code _ml_poll_for_launchers(ml_controller_t *cont);
ml_error_code _ml_update_launchers(ml_controller_t *,
    struct libusb_device **, int);
ml_error_code _ml_get_launchers_from_devices(libusb_device **,
    int, libusb_device ***, uint32_t *);
ml_error_code _ml_remove_disconnected_launchers(ml_controller_t *,
    libusb_device **, uint32_t);
ml_error_code _ml_add_new_launchers(ml_controller_t *,
    libusb_device **, uint32_t *);

// Launcher Array
ml_error_code _ml_remove_launcher(ml_controller_t *, ml_launcher_t *);
ml_error_code _ml_remove_launcher_index(ml_controller_t *, int16_t);
ml_error_code _ml_add_launcher(ml_controller_t *, ml_launcher_t *);
ml_error_code _ml_add_launcher_index(ml_controller_t *,
    ml_launcher_t *, int16_t);

// Launcher Init
ml_error_code _ml_launcher_init(ml_controller_t *,
    ml_launcher_t *, libusb_device *);
ml_error_code _ml_launcher_cleanup(ml_launcher_t **);
uint8_t _ml_catagorize_device(struct libusb_device_descriptor *);

// Launcher Control
ml_error_code ml_usb_open_launcher(ml_launcher_t *launcher);
ml_error_code ml_usb_close_launcher(ml_launcher_t *launcher);
ml_error_code _ml_launcher_move_unsafe(ml_launcher_t *, ml_launcher_direction);
ml_error_code _ml_launcher_move_time_unsafe(ml_launcher_t *,
    ml_launcher_direction, ml_time_t *);
ml_error_code _ml_launcher_send_cmd_unsafe(ml_launcher_t *, ml_launcher_cmd);

// Time Conversions
ml_error_code _ml_mseconds_to_time(uint32_t, ml_time_t *);

#ifdef __cplusplus
}
#endif

#endif
