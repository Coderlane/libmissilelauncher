/**
 * @file thunder-launcher.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-15
 */

#include "tl_control.h"


/**
 * @brief Moves the specified launcher in some direction for some amount of time
 *
 * @param launcher The launcher to move
 * @param direction The direction to move the launcher
 * @param time How long to move in some direction in milliseconds
 *
 * @return A status code
 */
int16_t move_time(thunder_launcher *launcher, tl_direction direction, uint32_t time) {

  return TL_NOT_IMPLEMENTED;
}

/**
 * @brief Moves the specified launcher in some direction for some specified degrees 
 *
 * @param launcher The launcher to move
 * @param direction The direction to move the launcher
 * @param deg The number of digrees to move the launcher
 *
 * @return A status code
 */
int16_t move_degrees(thunder_launcher *launcher, tl_direction direction, int32_t deg) {

  return TL_NOT_IMPLEMENTED;
}

/**
 * @brief Sets the launcher to some vertical position 
 *
 * @param launcher The launcher to move
 * @param deg The angle versus the horizon to set the launcher to.
 *
 * @return 
 */
int16_t set_vertical_position(thunder_launcher *launcher, int32_t deg) {

  return TL_NOT_IMPLEMENTED;
}


/**
 * @brief Sets the launcher to some horizontal position 
 *
 * @param launcher The launcher to move
 * @param deg The angle versus the horizon to set the launcher to.
 *
 * @return 
 */
int16_t set_horizontal_position(thunder_launcher *launcher, int32_t deg) {

  return TL_NOT_IMPLEMENTED;
}

/**
 * @brief Sets the launcher's position to 0 degrees horizontal, 0 degrees vertical. 
 *
 * @param launcher The launcher to zero
 *
 * @return 
 */ 
int16_t zero(thunder_launcher *launcher) {
  int16_t result = 0;

  result = set_horizontal_position(launcher, 0);
  if(result != TL_OK)
    return result;

  return set_vertical_position(launcher, 0);
}


/**
 * @brief Turns the led of the launcher on
 *
 * @param launcher The launcher to turn on
 *
 * @return 
 */
int16_t led_on(thunder_launcher *launcher) {

  return TL_NOT_IMPLEMENTED;
}


/**
 * @brief Turns the led of the launcher off
 *
 * @param launcher The launcher to turn off
 *
 * @return 
 */
int16_t led_off(thunder_launcher *launcher) {

  return TL_NOT_IMPLEMENTED;
}


/**
 * @brief Fires a missile
 *
 * @param launcher The launcher to fire a missile from
 *
 * @return 
 */
int16_t fire(thunder_launcher *launcher) {

  return TL_NOT_IMPLEMENTED;
}
