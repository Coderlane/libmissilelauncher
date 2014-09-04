/**
 * @file ml_launcher.c
 * @brief Functions for control over missile launchers.
 * @author Travis Lane
 * @version 0.2.0
 * @date 2014-05-18
 */

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
int16_t _ml_init_launcher(ml_controller_t *controller,
		ml_launcher_t *launcher, libusb_device *device) {

	struct libusb_device_descriptor desc;
	
	if (launcher == NULL) {
		TRACE("Launcher was null. _ml_init_launcher\n");
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
int16_t _ml_cleanup_launcher(ml_launcher_t **launcher) {
	if ((*launcher) == NULL) {
		TRACE("Launcher was null. _ml_cleanup_launcher\n");
		return ML_NULL_LAUNCHER;
	}


	free((*launcher));
	launcher = NULL;
	return ML_OK;
}

int16_t ml_usb_open_launcher(ml_launcher_t *launcher) {
	int status;

	if(launcher->is_open) {
		WARNING("Launcher was already open.\n");
		return 1;
	}

	status = libusb_open(launcher->usb_device, &(launcher->usb_handle));
	if(status != 0) {
		WARNING("Failed to open device descriptor, do you have permissions?\n");
		TRACE("Error Code: %d\n", status);
		return status;
	}
#ifdef LINUX
	// Linux needs some workarounds
	status = libusb_kernel_driver_active(launcher->usb_handle, 0);
	if(status == 1) {
		libusb_detach_kernel_driver(launcher->usb_handle, 0);
	}
	libusb_claim_interface(launcher->usb_handle, 0);
#endif

	return ML_OK;
}

int16_t ml_usb_close_launcher(ml_launcher_t *launcher) {
	if(!(launcher->is_open)) {
		goto out;
	}	
#ifdef LINUX
	libusb_release_interface(launcher->usb_handle, 0);
#endif
	libusb_close(launcher->usb_handle);
out:
	launcher->is_open = 0;
	return ML_OK;
}

/**
 * @brief Refrences a launcher so that it isn't destroyed.
 * If a launcher isn't refrenced, when you call ml_free_launcher_array the
 * refrence may
 * or may not be freed. Thus you can't guarantee that the memory is still there
 * for
 * you to use. Using ml_refrence_launcher with ml_dereference_launcher avoids
 * this issue.
 *
 * @param launcher The launcher to reference.
 *
 * @return A status code.
 */
int16_t ml_reference_launcher(ml_launcher_t *launcher) {
	if (launcher == NULL) {
		TRACE("Launcher was null. ml_refrence_launcher\n");
		return ML_NULL_LAUNCHER;
	}
	//pthread_mutex_lock(&(launcher->main_lock));
	launcher->ref_count += 1;
	//pthread_mutex_unlock(&(launcher->main_lock));
	return ML_OK;
}

/**
 * @brief Dereferences a launcher so that it can be eventually destroyed.
 * Just as with malloc and free, when using ml_reference_launcher you must use
 * ml_dereference_launcher or suffer memory leaks.
 *
 * @param launcher The launcher to dereference.
 *
 * @return A status code.
 */
int16_t ml_dereference_launcher(ml_launcher_t *launcher) {
	if (launcher == NULL) {
		TRACE("Launcher was null. ml_derefrence_launcher\n");
		return ML_NULL_LAUNCHER;
	}
	//pthread_mutex_lock(&(launcher->main_lock));
	launcher->ref_count -= 1;
	if (launcher->ref_count == 0 && launcher->device_connected == 0) {
		// Not connected and not refrenced
		_ml_remove_launcher(launcher->controller, launcher);
		_ml_cleanup_launcher(&launcher);
	}
	//pthread_mutex_unlock(&(launcher->main_lock));
	return ML_OK;
}

/**
 * @brief Fires a missile from the launcher.
 *
 * @param launcher The launcher to fire from.
 *
 * @return A status code.
 */
int16_t ml_fire_launcher(ml_launcher_t *launcher) {
	int16_t result = 0;
	
	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}
	// TODO implement error checking
	result = _ml_send_command_unsafe(launcher, ML_FIRE_CMD);

out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Stop moving the launcher.
 *
 * @param launcher The launcher to stop. 
 *
 * @return A status code.
 */
int16_t ml_stop_launcher(ml_launcher_t *launcher) {
	int16_t result = 0;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}	
	
	result = _ml_send_command_unsafe(launcher, ML_STOP_CMD);

out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Moves the launcher in the specified direction.
 *
 * @param launcher The launcher to move.
 * @param direction The specified direction.
 *
 * @return A status code.
 */
int16_t ml_move_launcher(ml_launcher_t *launcher,
		ml_launcher_direction direction) {
	int16_t result = 0;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}

	result = _ml_move_launcher_unsafe(launcher, direction);
	
out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Resets the launcher to 0 degrees of elevation and then centers the
 * launcher.
 *
 * @param launcher The launcher to zero.
 *
 * @return A status code.
 */
int16_t ml_zero_launcher(ml_launcher_t *launcher) {
	ml_time_t left_time, down_time, right_time, up_time;
	int result = ML_OK;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}
	TRACE("Starting to zero\n");

	//pthread_mutex_lock(&(launcher->main_lock));

	switch (launcher->type) {
		case ML_STANDARD_LAUNCHER:
			// Setup movement constants
			_ml_mseconds_to_time(6000, &left_time); // Correct
			_ml_mseconds_to_time(2000, &down_time); // Correct
			_ml_mseconds_to_time(2750, &right_time);
			_ml_mseconds_to_time(100, &up_time); // Correct
			break;
		default:
			TRACE("Unknown type.\n");
			return ML_NOT_IMPLEMENTED;
	}

	TRACE("Zeroing!\n");

	// Move to known position then to 0 deg vert and center
	_ml_move_launcher_time_unsafe(launcher, ML_LEFT, &left_time);
	_ml_move_launcher_time_unsafe(launcher, ML_DOWN, &down_time);
	_ml_move_launcher_time_unsafe(launcher, ML_RIGHT, &right_time);
	_ml_move_launcher_time_unsafe(launcher, ML_UP, &up_time);

out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Turns on the led of the selected launcher.
 *
 * @param launcher The launcher to get it's led turned on.
 *
 * @return A status code.
 */
int16_t ml_led_on(ml_launcher_t *launcher) {
	int16_t result = 0;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}

	// TODO implement error checking
	result = _ml_send_command_unsafe(launcher, ML_LED_ON_CMD);
	launcher->led_status = 1;

out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Turns off the led of the selected launcher.
 *
 * @param launcher The launcher to get it's led turned off.
 *
 * @return A status code.
 */
int16_t ml_led_off(ml_launcher_t *launcher) {
	int16_t result = 0;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}

	// TODO implement error checking
	result = _ml_send_command_unsafe(launcher, ML_LED_OFF_CMD);
	launcher->led_status = 0;

out:
	ml_usb_close_launcher(launcher);
	return result;
}

/**
 * @brief Determines if the LED is on or off.
 *
 * @param launcher The launcher to check.
 *
 * @return 1 = on 0 = off
 */
uint8_t ml_get_led_stat(ml_launcher_t *launcher) {
	uint8_t status = 0;

	status = launcher->led_status;

	return status;
}

/**
 * @brief Moves the specified launcher, in the specified direction
 * for the specified number of milliseconds. Don't use this in combination
 * with ml_move_launcher_degrees without zeroing the launcher afterwards.
 *
 * @param launcher The launcher the move.
 * @param direction The direction to move in.
 * @param mseconds The number of milliseconds to move for.
 *
 * @return A status code.
 */
int16_t ml_move_launcher_mseconds(ml_launcher_t *launcher,
		ml_launcher_direction direction,
		uint32_t mseconds) {

	int16_t result = 0;
	ml_time_t time;

	result = ml_usb_open_launcher(launcher);
	if(result != ML_OK) {
		goto out;
	}

	_ml_mseconds_to_time(mseconds, &time);
	result = _ml_move_launcher_time_unsafe(launcher, direction, &time);

out:
	ml_usb_close_launcher(launcher);
	return result;
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
int16_t _ml_move_launcher_time_unsafe(ml_launcher_t *launcher,
		ml_launcher_direction direction,
		ml_time_t *time) {

	int16_t result = 0;

	result = _ml_move_launcher_unsafe(launcher, direction);
	if (result != ML_OK) {
		goto out;
	}
	// Sleep the set amount of time
	ml_second_sleep(time->seconds);
	ml_msecond_sleep(time->mseconds);
	// Stop movement
	result = _ml_send_command_unsafe(launcher, ML_STOP_CMD);
	// Wait for device to stop coasting
	ml_msecond_sleep(200);

out:
	return result;
}

int16_t _ml_move_launcher_unsafe(ml_launcher_t *launcher, 
		ml_launcher_direction direction) {
	return _ml_send_command_unsafe(launcher, (ml_launcher_cmd)direction);
}

/**
 * @brief Sends a command to the launcher.
 * Add to this switch statement if you have a different type of launcher.
 *
 * @param launcher The launcher to send the command to.
 * @param cmd The command to send to the launcher.
 *
 * @return A status code.
 */
int16_t _ml_send_command_unsafe(ml_launcher_t *launcher, ml_launcher_cmd cmd) {
	uint8_t request_type = 0, request_field = 0;
	uint16_t wValue = 0, wIndex = 0;
	int16_t status = 0;
	switch (launcher->type) {
		case ML_STANDARD_LAUNCHER:
			request_type = ML_REQUEST_TYPE_SEND;
			request_field = ML_REQUEST_FIELD_SEND;
			wValue = 0;
			wIndex = 0;
			break;
		default:
			return ML_NOT_IMPLEMENTED;
	}

	status = libusb_control_transfer(launcher->usb_handle, request_type, request_field,
			wValue, wIndex, ml_cmd_arr[cmd], ML_CMD_ARR_SIZE, 0);
	if (status < 0) {
		WARNING("Error sending command.\n");
		TRACE("Error Code: %d\n", status);
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
int16_t _ml_mseconds_to_time(uint32_t mseconds, ml_time_t *time) {
	if (time == NULL)
		return ML_NULL_POINTER;
	// Time conversion
	time->seconds = (mseconds / 1000);                  // Get the seconds
	time->mseconds = mseconds - (time->seconds * 1000); // Get leftovers
	return ML_OK;
}

/**
 * @brief Changes degrees to a time object
 * Not yet implemented
 *
 * @param degrees The degrees to convert.
 * @param time The time object to put the result into.
 *
 * @return A status code.
 */
int16_t _ml_degrees_to_time(uint16_t degrees, ml_time_t *time) {
	time->mseconds = 0;
	// Silence warning
	time->seconds = degrees;
	time->seconds = 0;

	return ML_NOT_IMPLEMENTED;
}

/**
 * @brief Gets the type of launcher from the launcher.
 *
 * @param launcher The launcher to check.
 *
 * @return The launcher type.
 */
ml_launcher_type ml_get_launcher_type(ml_launcher_t *launcher) {
	ml_launcher_type type;

	//pthread_mutex_lock(&(launcher->main_lock));

	// Grab the type
	type = launcher->type;

	//pthread_mutex_unlock(&(launcher->main_lock));
	return type;
}
