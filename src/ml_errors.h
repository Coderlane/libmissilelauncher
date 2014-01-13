/**
 * @file ml_errors.h
 * @brief All errors used in the library. 
 * @author Travis Lane
 * @version 0.1.0
 * @date 2013-12-14
 */

#ifndef ML_ERRORS_H
#define ML_ERRORS_H

///< Error codes used in the library an enumeration is used
typedef enum ml_error_codes {
	ML_OK, ///< Everything is OK
	ML_NOT_IMPLEMENTED, ///< Specific function not implemented.
	ML_TEST_ERROR, ///< A test failed.
	ML_LIBRARY_ALREADY_INIT, ///< Library was already initialized. ml_init_library called more than once.
	ML_LIBRARY_NOT_INIT, ///< Library was not initialized, call ml_init_library before using any function.
	ML_LIBUSB_INIT_FAILED,///< libusb-1.0 failed to initialize.
	ML_INVALID_POLL_RATE,///< An invalid poll rate was specified, try a value between 1 and 120 or 0 for default (2).
	ML_FAILED_POLL_START,///< Polling failed to start.
	ML_NULL_LAUNCHER,///< A launcher was expected, but a null pointer was found.
	ML_POSITION_WAS_NOT_NULL,///< The specified position was not null when a null position was expected.
	ML_POSITION_WAS_NULL,///< The specified position was null when a valid pointer was expected.
	ML_INDEX_OUT_OF_BOUNDS,///< An array went past the end .
	ML_COUNT_ZERO,///< No items are in the array so it would be impossible to remove one. 
	ML_NOT_FOUND,///< An item was not found.
	ML_ARRAY_NOT_NULL,///< A null pointer was expected, but one with possible data was encountered. 
	ML_ALLOC_FAILED,///< An allocate failed. Having memory issues?
	ML_LAUNCHER_ARRAY_INCONSISTENT,///< The launcher array is in an inconsisten state. This is the library's fault. Please file a bug report.
	ML_NULL_POINTER,///< A requred pointer was null.
	ML_NO_LAUNCHERS///< No launchers were detected.
} ml_error_codes;

#endif
