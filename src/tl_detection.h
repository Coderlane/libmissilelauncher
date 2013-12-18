/**
 * @file tl_detection.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */

#ifndef TL_DETECTION_H
#define TL_DETECTION_H

#include "tl_common.h"

int16_t initialize_library();
int16_t cleanup_library();

int16_t _initialize_control(launch_control *init_control);
int16_t _cleanup_control(launch_control *clean_control);

int16_t poll_for_launcher();

#endif
