/**
 * @file thunder-launcher.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-14
 */

#ifndef TL_DETECTION_H
#define TL_DETECTION_H

#include "tl_common.h"

int16_t initialize_library();
int16_t cleanup_library();
int16_t poll_for_launcher();

#endif
