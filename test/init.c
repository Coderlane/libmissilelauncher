/**
 * @file init.c
 * @brief Test sources for tests dealing with
 * the initilization and cleanup of the library
 * @author Travis Lane
 * @version 0.0.1
 * @date 2013-12-18
 */

#include "init.h"

int main() {
  char response = '\0';
  uint8_t run_tests = 1;
  fprintf(stderr, "****Running initialization and cleanup tests.****\n");
  for(uint16_t i = 0; i < MAX_TEST_INDEX && run_tests; i++) {
    if(test_array[i]() != TL_OK) {
      uint8_t invalid = 1;
      while(invalid) {
        fprintf(stderr, "Test failed, continue? (y/n)");
        response = getchar(); 
        getchar(); //Eat newline 
        if(!isalpha(response)) 
          continue;
        response = tolower(response);
        if(response == 'y') {
          invalid = 0;
        } else if(response == 'n') {
          invalid = 0;
          run_tests = 0;
        } 
      }
    }
  }
  fprintf(stderr, "****Done running initialization and clean up tests.****\n");
  return TL_OK;
}

int16_t test_init() {
  fprintf(stderr, "test_init: Not implemented\n");
  return TL_NOT_IMPLEMENTED;
}

int16_t test_cleanup() {
  fprintf(stderr, "test_cleanup: Not implemented\n");
  return TL_NOT_IMPLEMENTED;
}

int16_t test_double_clean() {
  int16_t result = 0;
  initialize_library();
  cleanup_library();
  result = cleanup_library();
  if(result == TL_CONTROL_ALREADY_NULL) {
    fprintf(stderr, "test_double_clean: OK\n");
    return TL_OK;
  }
  fprintf(stderr, "test_double_clean: Output was not expected value. Expected %d Recieved %d\n", TL_LIBRARY_ALREADY_INIT, result);
  return TL_TEST_ERROR;
}

int16_t test_double_init() {
  int16_t result = 0;
  initialize_library();
  result = initialize_library();
  cleanup_library();

  if(result == TL_LIBRARY_ALREADY_INIT) {
    fprintf(stderr, "test_double_init: OK\n");
    return TL_OK;
  }
  fprintf(stderr, "test_double_init: Output was not expected value. Expected %d Recieved %d\n", TL_LIBRARY_ALREADY_INIT, result);
  return TL_TEST_ERROR;
}



