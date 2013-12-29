
#include "detection.h"

int main() {
  char response = '\0';
  uint8_t run_tests = 1;
  initialize_library();
  fprintf(stderr, "****Running usb detection tests.****\n");
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
  cleanup_library();
  fprintf(stderr, "****Done running usb detection tests.****\n");
  return TL_OK;
}


int16_t test_constant_poll() {
  start_continuous_poll();
  second_sleep(10000);
  return TL_NOT_IMPLEMENTED;
}

