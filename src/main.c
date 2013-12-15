/**
 * @file main.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-15
 */

#include "thunder-launcher.h"
#include "tlib_debug.h"

int main() {

  if(tl_down_command[0] == _TL_COM_VAL) {
    TRACE("test value matches\n");
  }
  return 0;
}


