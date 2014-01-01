/**
 * @file ml_errors.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-14
 */

#ifndef ML_ERRORS_H
#define ML_ERRORS_H


typedef enum ml_error_codes {
  ML_OK,
  ML_NOT_IMPLEMENTED,
  ML_TEST_ERROR,
  ML_LIBRARY_ALREADY_INIT,
  ML_CONTROL_ALREADY_INIT,
  ML_CONTROL_ALREADY_NULL, 
  ML_CONTROL_WAS_NULL,
  ML_CONTROL_ALLOC_FAIL,
  ML_CONTROL_ARR_ALLOC_FAIL,
  ML_CONTROL_FREE_NULL,
  ML_CONTROL_ARR_NULL,
  ML_LIBUSB_INIT_FAILED
} ml_error_codes;

#endif
