/**
 * @file ml_controller.c
 * @brief Functions releated to the launcher controller
 * @author Travis Lane
 * @version 0.5.0
 * @date 2016-11-27
 */

#include <stdint.h>
#include <stdlib.h>

#include "libmissilelauncher.h"
#include "libmissilelauncher_internal.h"

/**
 * @brief initializes a controller.
 *
 * @param controller The controller to initialize.
 *
 * @return A status code.
 */
ml_error_code
_ml_controller_init(ml_controller_t *controller)
{
  if (controller->control_initialized) {
    return ML_LIBRARY_ALREADY_INIT;
  }
  // Setup the array
  controller->launchers =
    calloc(sizeof(ml_arr_launcher_t), ML_INITIAL_LAUNCHER_ARRAY_SIZE);

  if (controller->launchers == NULL) {
    return ML_ALLOC_FAILED;
  }
  // Set default variables
  controller->launcher_array_size = ML_INITIAL_LAUNCHER_ARRAY_SIZE;
  controller->launcher_count = 0;
  // Good to go!
  controller->control_initialized = 1;
  return ML_OK;
}



/**
 * @brief Cleans up the controller when the library is done.
 *
 * @param controller The controller to clean up.
 *
 * @return A status code.
 */
ml_error_code
_ml_controller_cleanup(ml_controller_t *controller)
{
  ml_launcher_t *cur_launcher = NULL;
  // Error checking
  if (controller == NULL) {
    return ML_LIBRARY_NOT_INIT;
  }

  if (controller->launchers == NULL) {
    return ML_LAUNCHER_ARRAY_INCONSISTENT;
  }
  // Cleaning up the library. Free up every launcher.
  for (int16_t i = 0; i < controller->launcher_array_size; i++) {
    cur_launcher = controller->launchers[i];
    if (cur_launcher != NULL) {
      // Free current launcher
      _ml_launcher_cleanup(&cur_launcher);
    }
  }

  // Clean up array
  free(controller->launchers);
  controller->launchers = NULL;
  controller->launcher_array_size = 0;
  controller->launcher_count = 0;
  // Controll is no longer initialized
  controller->control_initialized = 0;
  return ML_OK;
}

/**
 * @brief Polls for new launchers.
 *
 * @param cont The controller to poll.
 *
 * @return A status code.
 */
ml_error_code
_ml_poll_for_launchers(ml_controller_t *cont)
{
  int device_count = 0;
  ml_error_code status = 0;
  libusb_device **devices = NULL;
  device_count = libusb_get_device_list(NULL, &devices);
  status = _ml_update_launchers(cont, devices, device_count);
  libusb_free_device_list(devices, 1);
  return status;
}

/**
 * @brief This function catagorizes the type of launcher.
 * For now we only support:
 * Thunder Launchers - ML_STANDARD_LAUNCHER
 *
 * @param desc The USB device descriptor.
 *
 * @return They type of launcher or ML_NOT_LAUNCHER if it isn't a launcher.
 */
uint8_t
_ml_catagorize_device(struct libusb_device_descriptor *desc)
{
  if (desc->idProduct == ML_STD_PRODUCT_ID &&
      desc->idVendor == ML_STD_VENDOR_ID) {
    return ML_STANDARD_LAUNCHER;
  }
  return ML_NOT_LAUNCHER;
}

/**
 * @brief Checks the USB device array and sees if any devices were added or
 * removed.
 * If any devices were added or removed we update the array.
 *
 * @param cont The active controller.
 * @param devices The devices libusb found.
 * @param device_count the number of devices found.
 *
 * @return A status code, ML_OK if everything went well.
 */
ml_error_code
_ml_update_launchers(ml_controller_t *cont,
                     struct libusb_device **devices, int device_count)
{

  libusb_device **found_launchers = NULL;
  uint32_t found_launchers_count = 0;

  _ml_get_launchers_from_devices(devices, device_count, &found_launchers,
                                 &found_launchers_count);


  _ml_remove_disconnected_launchers(cont,
                                    found_launchers, found_launchers_count);

  _ml_add_new_launchers(cont, found_launchers, &found_launchers_count);

  free(found_launchers);

  cont->launcher_count = found_launchers_count;
  return ML_OK;
}

/**
 * @brief Determines what devices returned by libusb were launcher devices.
 * Collects them and adds them to an array.
 *
 * @param devices The devices detected by libusb.
 * @param device_count The number of devices detected.
 * @param found_launchers The launchers detected.
 * @param found_launchers_count The number of launchers detected.
 *
 * @return A status code.
 */
ml_error_code
_ml_get_launchers_from_devices(libusb_device **devices,
                               int device_count,
                               libusb_device ***found_launchers,
                               uint32_t *found_launchers_count)
{

  libusb_device *found_device = NULL;
  uint32_t found_count = 0;

  (*found_launchers_count) = 0;

  // Make a new array of devices
  for (int i = 0;
       i < device_count && (found_device = devices[i]) != NULL; i++) {

    struct libusb_device_descriptor device_descriptor;
    libusb_get_device_descriptor(found_device, &device_descriptor);

    // Check if the device is a launcher
    if (_ml_catagorize_device(&device_descriptor) != ML_NOT_LAUNCHER) {
      // Device is launcher
      found_count += 1;
      // Resize array
      (*found_launchers) = realloc((*found_launchers),
                                   (found_count + 1) * sizeof(libusb_device *));

      if ((*found_launchers) == NULL) {
        return ML_ALLOC_FAILED;
      }
      // Add to array and set last to null.
      (*found_launchers)[found_count - 1] = found_device;
      (*found_launchers)[found_count] = NULL;
    }
  }

  (*found_launchers_count) = found_count;
  return ML_OK;
}

/**
 * @brief Sets launchers that have been removed as such and frees them if
 * proper.
 *
 * @param cont The active controller.
 * @param found_launchers An array of libusb_devices which are all launchers.
 * @param found_launchers_count The number of launchers in the array.
 *
 * @return A status code.
 */
ml_error_code
_ml_remove_disconnected_launchers(ml_controller_t *cont,
                                  libusb_device **found_launchers,
                                  uint32_t found_launchers_count)
{

  libusb_device *found_device = NULL;
  ml_launcher_t *known_launcher = NULL;

  // Check to see if we need to remove any devices
  for (uint16_t known_it = 0;
       known_it < cont->launcher_array_size; known_it++) {
    // We'll be checking if the known device was not found in the new array of
    // mounted devices.
    uint8_t found = 0;

    known_launcher = cont->launchers[known_it];
    if (known_launcher == NULL) {
      continue;
    }

    // Check if any devices were found.
    if (found_launchers != NULL) {
      // Check the devices we found
      for (uint16_t found_it = 0;
           found_it < found_launchers_count &&
           (found_device = found_launchers[found_it]) != NULL && found == 0;
           found_it++) {

        if (known_launcher->usb_device == found_device) {
          found = 1;
        }
      }
    }
    // Update the connected state
    known_launcher->device_connected = found;

    if (known_launcher->device_connected == 0 &&
        known_launcher->ref_count == 0) {

      // No one is refrencing the device, so we can free it.
      _ml_remove_launcher_index(cont, known_it);
      _ml_launcher_cleanup(&known_launcher);
    } else {
    }
  }

  return ML_OK;
}

/**
 * @brief Adds new launchers to the main array.
 *
 * @param cont The active controller.
 * @param found_launchers The launchers found previously.
 * @param found_launchers_count The number of launchers found.
 *
 * @return A status code.
 */
ml_error_code
_ml_add_new_launchers(ml_controller_t *cont,
                      libusb_device **found_launchers,
                      uint32_t *found_launchers_count)
{

  libusb_device *found_device = NULL;
  ml_launcher_t *known_launcher = NULL;
  int16_t status = 0;
  uint32_t count = (*found_launchers_count);

  // Check for any new devices
  for (uint16_t found_it = 0;
       found_it < count && (found_device = found_launchers[found_it]) != NULL;
       found_it++) {
    // We'll be checking if the device is already present
    uint8_t found = 0;

    for (uint16_t known_it = 0;
         known_it < cont->launcher_array_size && found == 0;
         known_it++) {

      known_launcher = cont->launchers[known_it];
      if (known_launcher != NULL &&
          known_launcher->usb_device == found_device) {
        // Found something identical
        found = 1;
      }
    }
    if (found == 0) {
      // Device wasn't found in the array of known devices. Add it.
      ml_launcher_t *new_launcher = calloc(sizeof(ml_launcher_t), 1);
      if (new_launcher == NULL) {
      } else {
        status = _ml_launcher_init(cont, new_launcher, found_device);
        if (status == ML_OK) {
          _ml_add_launcher(cont, new_launcher);
        } else {
          free(new_launcher);
          (*found_launchers_count)--;
        }
      }
    }
  }
  return ML_OK;
}

/**
 * @brief Allocates space for and returns a new array of launchers.
 * To clean up use ml_free_launcher_array. Modifying values in the array
 * will produce undexpected results. Treat this array as a constant.
 * NOTE: if you don't want to lose access to a launcher use
 * ml_launcher_reference and ml_launcher_dereference when you are done.
 *
 * @param new_arr Pointer to where you want the array.
 * @param count The number of items found.
 *
 * @return A status code, ML_OK if everything went well.
 */
ml_error_code
ml_launcher_array_new(ml_launcher_t ***new_arr, uint32_t *count)
{
  ml_launcher_t *cur_launcher;
  int16_t new_index = 0, new_count = 0, status = 0;
  if (ml_library_is_init() == 0) {
    return ML_LIBRARY_NOT_INIT;
  }

  // Error checking
  if ((*new_arr) != NULL) {
    return ML_NOT_NULL_POINTER;
  }

  status = _ml_poll_for_launchers(ml_main_controller);
  if(status != ML_OK) {
    return status;
  }
  new_count = ml_main_controller->launcher_count;
  (*count) = new_count;
  if (new_count == 0) {
    return ML_NO_LAUNCHERS;
  }
  // Allocate space for the new array.
  (*new_arr) = malloc(sizeof(ml_launcher_t *) * (new_count + 1));
  if ((*new_arr) == NULL) {
    return ML_ALLOC_FAILED;
  }

  (*new_arr)[new_count] = NULL;

  // Find the launchers
  for (int16_t i = 0; i < ml_main_controller->launcher_array_size; i++) {
    // Found a launcher
    cur_launcher = ml_main_controller->launchers[i];
    if (cur_launcher != NULL) {
      if (new_index >= ml_main_controller->launcher_count) {
        return ML_LAUNCHER_ARRAY_INCONSISTENT;
      }
      // Refrence the launcher since this will be going back to the programmer
      ml_launcher_reference(cur_launcher);
      (*new_arr)[new_index] = cur_launcher;
      new_index += 1;
    }
  }

  return ML_OK;
}

/**
 * @brief Frees the array of launchers ml_get_launcher_array provides.
 * Dereferences every launcher in the array so they can be cleaned up later.
 * NOTE: if you don't want to lose access to a launcher use
 * ml_launcher_reference and ml_launcher_dereference when you are done.
 *
 * @param free_arr The array to free.
 *
 * @return A status code.
 */
ml_error_code
ml_launcher_array_free(ml_launcher_t **free_arr)
{
  ml_launcher_t *cur_launcher = NULL;
  int16_t index = 0;

  if (free_arr == NULL) {
    return ML_NULL_POINTER;
  }

  while ((cur_launcher = free_arr[index]) != NULL) {
    // Dereference each launcher.
    ml_launcher_dereference(cur_launcher);
    index++;
  }
  // Free the leftover array
  free(free_arr);
  return ML_OK;
}

/**
 * @brief Removes a launcher from the array.
 *
 * @param cont The active controller.
 * @param launcher The launcher to remove.
 *
 * @return A status value, ML_OK if it is okay.
 */
ml_error_code
_ml_remove_launcher(ml_controller_t *cont, ml_launcher_t *launcher)
{
  /* This function is not thread safe, please lock the array first */

  for (int16_t i = 0; i < cont->launcher_count; i++) {
    // Search for the launcher of interest
    if (cont->launchers[i] == launcher) {
      return _ml_remove_launcher_index(cont, i);
    }
  }
  return ML_NOT_FOUND;
}

/**
 * @brief Removes a launcher from the array.
 *
 * @param cont The active controller.
 * @param index The index of the item to remove.
 *
 * @return A status value, ML_OK if it is okay.
 */
ml_error_code
_ml_remove_launcher_index(ml_controller_t *cont, int16_t index)
{
  /* This function is not thread safe, please lock the array first */

  // Error checking
  if (cont->launchers[index] == NULL) {
    return ML_NULL_POINTER;
  }
  if (index >= cont->launcher_array_size || index < 0) {
    return ML_INDEX_OUT_OF_BOUNDS;
  }
  if (cont->launcher_count <= 0) {
    return ML_COUNT_ZERO;
  }

  // Everything looks good, decrement and set as null. We do not free here.
  cont->launcher_count -= 1;
  cont->launchers[index] = NULL;
  return ML_OK;
}

/**
 * @brief adds a launcher to the array.
 *
 * @param cont The active controller.
 * @param launcher The item to add.
 *
 * @return A status value, ML_OK if it is okay.
 */
ml_error_code
_ml_add_launcher(ml_controller_t *cont, ml_launcher_t *launcher)
{
  /* This function is not thread safe, please lock the array first */
  for (int16_t i = 0; i < cont->launcher_array_size; i++) {
    // Find an empty spot
    if (cont->launchers[i] == NULL) {
      return _ml_add_launcher_index(cont, launcher, i);
    }
  }

  // No freespace found, needs a resize.
  // Not yet implemented...
  return ML_NOT_IMPLEMENTED;
}

/**
 * @brief adds a launcher to the array.
 *
 * @param cont The active controller.
 * @param launcher The launcher to add.
 * @param index The index to insert it in.
 *
 * @return A status code, ML_OK if it is okay.
 */
ml_error_code
_ml_add_launcher_index(ml_controller_t *cont,
                       ml_launcher_t *launcher, int16_t index)
{
  /* This function is not thread safe, please lock the array first */
  if (cont->launchers[index] != NULL) {
    return ML_NOT_NULL_POINTER;
  }

  // Update index and add
  cont->launcher_count += 1;
  cont->launchers[index] = launcher;
  return ML_OK;
}
