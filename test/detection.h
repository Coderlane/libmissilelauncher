/**
 * @file detection.h
 * @brief Test header for detection of the launcher 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */

#ifndef TL_TEST_DETECTION_H
#define TL_TEST_DETECTION_H

#include <tl_detection.h>


int16_t test_constant_poll();

#define MAX_TEST_INDEX 1 

static int16_t (*test_array[MAX_TEST_INDEX])(void) = {test_constant_poll};




#endif
