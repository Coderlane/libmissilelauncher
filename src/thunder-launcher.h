/**
 * @file thunder-launcher.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-14
 */

#ifndef THUNDER_LAUNCHER_H
#define THUNDER_LAUNCHER_H

#include <stdint.h>


// Don't mess with these, they are constant for a reason.

// The communications array length
#define _TL_COM_ARR_LEN  10
// Value for all commands but LED commands
#define _TL_COM_VAL      0x02
// Value for LED Commands
#define _TL_LED_VAL      0x03

// Move down
#define _TL_DOWN_VAL     0x01
// Move up
#define _TL_UP_VAL       0x02
// Move left
#define _TL_LEFT_VAL     0x04
// Move right
#define _TL_RIGHT_VAL    0x08

// Fire at target
#define _TL_FIRE_VAL     0x10
// Stop moving
#define _TL_STOP_VAL     0x20

// Turn the LED on
#define _TL_LED_ON       0x00
// Turn the LEF off
#define _TL_LED_OFF      0x01

// The actual commands we are sending across the wire.
static char tl_down_command[_TL_COM_ARR_LEN] =    {_TL_COM_VAL, _TL_DOWN_VAL,   0, 0, 0, 0, 0, 0, 0};
static char tl_up_command[_TL_COM_ARR_LEN] =      {_TL_COM_VAL, _TL_UP_VAL,     0, 0, 0, 0, 0, 0, 0};
static char tl_left_command[_TL_COM_ARR_LEN] =    {_TL_COM_VAL, _TL_LEFT_VAL,   0, 0, 0, 0, 0, 0, 0};
static char tl_right_command[_TL_COM_ARR_LEN] =   {_TL_COM_VAL, _TL_RIGHT_VAL,  0, 0, 0, 0, 0, 0, 0};

static char tl_fire_command[_TL_COM_ARR_LEN] =    {_TL_COM_VAL, _TL_FIRE_VAL,   0, 0, 0, 0, 0, 0, 0};
static char tl_stop_command[_TL_COM_ARR_LEN] =    {_TL_COM_VAL, _TL_STOP_VAL,   0, 0, 0, 0, 0, 0, 0};

static char tl_led_on_command[_TL_COM_ARR_LEN] =  {_TL_LED_VAL, _TL_LED_ON,     0, 0, 0, 0, 0, 0, 0};
static char tl_led_off_command[_TL_COM_ARR_LEN] = {_TL_LED_VAL, _TL_LED_OFF,    0, 0, 0, 0, 0, 0, 0};

// Directions to move the turret
typedef enum tl_direction {
  TL_UP,
  TL_DOWN,
  TL_LEFT,
  TL_RIGHT
} tl_direction;

typedef struct thunder_launcher {
  uint8_t   led_status;
  uint8_t   missiles;
  uint32_t  horizontal_position;
  uint32_t  vertical_position;
} thunder_launcher;


int16_t move_time(thunder_launcher *launcher, tl_direction direction, uint32_t time);
int16_t move_degrees(thunder_launcher *launcher, tl_direction direction, uint32_t deg);

int16_t set_position(thunder_launcher *launcher, tl_direction direction, uint32_t deg);
int16_t zero(thunder_launcher *launcher);

int16_t led_on(thunder_launcher *launcher);
int16_t led_off(thunder_launcher *launcher);

int16_t fire(thunder_launcher *launcher);

typedef enum tl_launch_codes {
  TL_OK,
  TL_NOT_IMPLEMENTED
} tl_launch_codes;

#endif
