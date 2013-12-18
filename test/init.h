/**
 * @file init.h
 * @brief Test header for tests dealing with
 * the initilization and cleanup of the library
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */


#ifndef TL_TEST_INIT_H
#define TL_TEST_INIT_H

#include <tl_detection.h>

#define MAX_TEST_INDEX 4

int16_t test_init();
int16_t test_cleanup();

int16_t test_double_clean();
int16_t test_double_init();


static int16_t (*test_array[MAX_TEST_INDEX])(void) = { test_init, test_cleanup, test_double_init, test_double_clean};
#endif
