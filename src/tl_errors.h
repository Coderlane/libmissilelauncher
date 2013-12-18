/**
 * @file errors.h
 * @brief 
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-14
 */

#ifndef TL_ERRORS_H
#define TL_ERRORS_H


typedef enum tl_launch_codes {
  TL_OK,
  TL_NOT_IMPLEMENTED,
  TL_LIBRARY_ALREADY_INIT,
  TL_CONTROL_ALREADY_INIT,
  TL_CONTROL_ALREADY_NULL, 
  TL_CONTROL_ALLOC_FAIL,
  TL_CONTROL_ARR_ALLOC_FAIL,
  TL_CONTROL_FREE_NULL,
  TL_CONTROL_ARR_NULL
} tl_launch_codes;

#endif
