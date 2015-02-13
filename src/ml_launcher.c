/**
 * @file ml_launcher.c
 * @brief Functions for control over missile launchers.
 * @author Travis Lane
 * @version 0.3.0
 * @date 2014-05-18
 */

#include <stdint.h>
#include <stdlib.h>

#include "libmissilelauncher.h"
#include "libmissilelauncher_internal.h"

/**
 * @brief Initializes a newly connected launcher.
 *
 * @param controller The active controlle.
 * @param launcher The launcher to initialize.
 * @param device The device that was connected.
 *
 * @return A status code.
 */
int16_t
_ml_launcher_init(ml_controller_t *controller,
                  ml_launcher_t *launcher, libusb_device *device)
{

  struct libusb_device_descriptor desc;

  if (launcher == NULL) {
    return ML_NULL_LAUNCHER;
  }
  libusb_get_device_descriptor(device, &desc);

  launcher->type = _ml_catagorize_device(&desc);
  launcher->usb_device = device;
  launcher->ref_count = 0;
  launcher->device_connected = 1;
  launcher->controller = controller;

  return ML_OK;
}

/**
 * @brief Cleans up a launcher, This must be done so libusb devices are
 * released.
 *
 * @param launcher The launcher to destroy.
 *
 * @return A status code.
 */
int16_t
_ml_launcher_cleanup(ml_launcher_t **launcher)
{
  if ((*launcher) == NULL) {
    return ML_NULL_LAUNCHER;
  }


  free((*launcher));
  launcher = NULL;
  return ML_OK;
}

/**
 * @brief Claim the launcher.
 *
 * @param launcher The launcher to claim.
 *
 * @return A status code.
 */
int16_t
ml_launcher_claim(ml_launcher_t *launcher)
{
  int status;

  if(launcher->claimed) {
    return ML_OK;
  }

  status = libusb_open(launcher->usb_device, &(launcher->usb_handle));
  if(status != 0) {
    return status;
  }

	launcher->claimed = true;

  return ML_OK;
}

/**
 * @brief Unclaim the launcher.
 *
 * @param launcher The launcher to unclaim.
 *
 * @return A status code.
 */
int16_t
ml_launcher_unclaim(ml_launcher_t *launcher)
{
  if(!(launcher->claimed)) {
    goto out;
  }
  libusb_close(launcher->usb_handle);
out:
  launcher->claimed = false;
  return ML_OK;
}

/**
 * @brief Refrences a launcher so that it isn't destroyed.
 * If a launcher isn't refrenced, when you call ml_free_launcher_array the
 * refrence may
 * or may not be freed. Thus you can't guarantee that the memory is still there
 * for
 * you to use. Using ml_refrence_launcher with ml_launcher_dereference avoids
 * this issue.
 *
 * @param launcher The launcher to reference.
 *
 * @return A status code.
 */
int16_t
ml_launcher_reference(ml_launcher_t *launcher)
{
  if (launcher == NULL) {
    return ML_NULL_LAUNCHER;
  }
  launcher->ref_count += 1;
  return ML_OK;
}

/**
 * @brief Dereferences a launcher so that it can be eventually destroyed.
 * Just as with malloc and free, when using ml_launcher_reference you must use
 * ml_launcher_dereference or suffer memory leaks.
 *
 * @param launcher The launcher to dereference.
 *
 * @return A status code.
 */
int16_t
ml_launcher_dereference(ml_launcher_t *launcher)
{
  if (launcher == NULL) {
    return ML_NULL_LAUNCHER;
  }
  launcher->ref_count -= 1;
  if (launcher->ref_count == 0 && launcher->device_connected == 0) {
    // Not connected and not refrenced
    _ml_remove_launcher(launcher->controller, launcher);
    _ml_launcher_cleanup(&launcher);
  }
  return ML_OK;
}

/**
 * @brief Fires a missile from the launcher.
 *
 * @param launcher The launcher to fire from.
 *
 * @return A status code.
 */
int16_t
ml_launcher_fire(ml_launcher_t *launcher)
{
  if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  // TODO implement error checking
  return _ml_launcher_send_cmd_unsafe(launcher, ML_FIRE_CMD);
}

/**
 * @brief Stop moving the launcher.
 *
 * @param launcher The launcher to stop.
 *
 * @return A status code.
 */
int16_t
ml_launcher_stop(ml_launcher_t *launcher)
{
  if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  return _ml_launcher_send_cmd_unsafe(launcher, ML_STOP_CMD);
}

/**
 * @brief Moves the launcher in the specified direction.
 *
 * @param launcher The launcher to move.
 * @param direction The specified direction.
 *
 * @return A status code.
 */
int16_t
ml_launcher_move(ml_launcher_t *launcher, ml_launcher_direction direction)
{
  if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  return _ml_launcher_move_unsafe(launcher, direction);
}

/**
 * @brief Resets the launcher to 0 degrees of elevation and then centers the
 * launcher.
 *
 * @param launcher The launcher to zero.
 *
 * @return A status code.
 */
int16_t
ml_launcher_zero(ml_launcher_t *launcher)
{
  ml_time_t left_time, down_time, right_time, up_time;

  if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  switch (launcher->type) {
  case ML_STANDARD_LAUNCHER:
    // Setup movement constants
    _ml_mseconds_to_time(6000, &left_time); // Correct
    _ml_mseconds_to_time(2000, &down_time); // Correct
    _ml_mseconds_to_time(2750, &right_time);
    _ml_mseconds_to_time(100, &up_time); // Correct
    break;
  default:
    return ML_NOT_IMPLEMENTED;
  }

  // Move to known position then to 0 deg vert and center
  _ml_launcher_move_time_unsafe(launcher, ML_LEFT, &left_time);
  _ml_launcher_move_time_unsafe(launcher, ML_DOWN, &down_time);
  _ml_launcher_move_time_unsafe(launcher, ML_RIGHT, &right_time);
  _ml_launcher_move_time_unsafe(launcher, ML_UP, &up_time);

  return ML_OK;
}

/**
 * @brief Turns on the led of the selected launcher.
 *
 * @param launcher The launcher to get it's led turned on.
 *
 * @return A status code.
 */
int16_t
ml_launcher_led_on(ml_launcher_t *launcher)
{
  int16_t result = 0;

	if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  // TODO implement error checking
  result = _ml_launcher_send_cmd_unsafe(launcher, ML_LED_ON_CMD);
  if(result == ML_OK) {
    launcher->led_status = 1;
  }

  return result;
}

/**
 * @brief Turns off the led of the selected launcher.
 *
 * @param launcher The launcher to get it's led turned off.
 *
 * @return A status code.
 */
int16_t
ml_launcher_led_off(ml_launcher_t *launcher)
{
  int16_t result = 0;

	if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  // TODO implement error checking
  result = _ml_launcher_send_cmd_unsafe(launcher, ML_LED_OFF_CMD);
  if(result == ML_OK) {
    launcher->led_status = 0;
  }

  return result;
}

/**
 * @brief Determines if the LED is on or off.
 *
 * @param launcher The launcher to check.
 *
 * @return 1 = on 0 = off
 */
uint8_t
ml_launcher_get_led_state(ml_launcher_t *launcher)
{
  uint8_t status = 0;

  status = launcher->led_status;

  return status;
}

/**
 * @brief Moves the specified launcher, in the specified direction
 * for the specified number of milliseconds.
 *
 * @param launcher The launcher the move.
 * @param direction The direction to move in.
 * @param mseconds The number of milliseconds to move for.
 *
 * @return A status code.
 */
int16_t
ml_launcher_move_mseconds(ml_launcher_t *launcher,
                          ml_launcher_direction direction,
                          uint32_t mseconds)
{
  ml_time_t time;

	if(!launcher->claimed) {
    return ML_UNCLAIMED;
  }

  _ml_mseconds_to_time(mseconds, &time);
  return _ml_launcher_move_time_unsafe(launcher, direction, &time);
}

/**
 * @brief Moves the launcher for a specified amount of time.
 *
 * @param launcher Th launcher to move.
 * @param direction The direction to move in.
 * @param time The time to move for.
 *
 * @return A status code
 */
int16_t
_ml_launcher_move_time_unsafe(ml_launcher_t *launcher,
                              ml_launcher_direction direction,
                              ml_time_t *time)
{
  int16_t result = 0;

  result = _ml_launcher_move_unsafe(launcher, direction);
  if (result != ML_OK) {
    goto out;
  }
  // Sleep the set amount of time:
  ml_second_sleep(time->seconds);
  ml_msecond_sleep(time->mseconds);
  // Stop movement
  result = _ml_launcher_send_cmd_unsafe(launcher, ML_STOP_CMD);
  // Wait for device to stop coasting
  ml_msecond_sleep(200);

out:
  return result;
}

/**
 * @brief Move the launcher in an unsafe manner.
 * Internal use only please! :)
 *
 * @param launcher The launcher to move.
 * @param direction The direction to move the launcher in.
 *
 * @return A status code.
 */
int16_t
_ml_launcher_move_unsafe(ml_launcher_t *launcher,
                         ml_launcher_direction direction)
{
  return _ml_launcher_send_cmd_unsafe(launcher, (ml_launcher_cmd)direction);
}

/**
 * @brief Sends a cmd to the launcher.
 * Add to this switch statement if you have a different type of launcher.
 *
 * @param launcher The launcher to send the cmd to.
 * @param cmd The cmd to send to the launcher.
 *
 * @return A status code.
 */
int16_t
_ml_launcher_send_cmd_unsafe(ml_launcher_t *launcher, ml_launcher_cmd cmd)
{
  uint8_t request_type = 0, request_field = 0;
  uint16_t value = 0, index = 0;
  int16_t status = 0;
  switch (launcher->type) {
  case ML_STANDARD_LAUNCHER:
    request_type = ML_REQUEST_TYPE_SEND;
    request_field = ML_REQUEST_FIELD_SEND;
    value = 0;
    index = 0;
    break;
  default:
    return ML_NOT_IMPLEMENTED;
  }

  status = libusb_control_transfer(launcher->usb_handle, request_type,
                                   request_field, value, index,
                                   ml_cmd_arr[cmd], ML_CMD_ARR_SIZE, 0);
  if (status < 0) {
    return status;
  } else {
    return ML_OK;
  }
}

/**
 * @brief Changes milliseconds to a time object.
 *
 * @param mseconds The milliseonds to convert.
 * @param time The time object to put it into.
 *
 * @return A status code.
 */
int16_t
_ml_mseconds_to_time(uint32_t mseconds, ml_time_t *time)
{
  if (time == NULL) {
    return ML_NULL_POINTER;
  }
  // Time conversion
  time->seconds = (mseconds / 1000);                  // Get the seconds
  time->mseconds = mseconds - (time->seconds * 1000); // Get leftovers
  return ML_OK;
}

/**
 * @brief Gets the type of launcher from the launcher.
 *
 * @param launcher The launcher to check.
 *
 * @return The launcher type.
 */
ml_launcher_type
ml_launcher_get_type(ml_launcher_t *launcher)
{
  ml_launcher_type type;
  // Grab the type
  type = launcher->type;
  return type;
}
