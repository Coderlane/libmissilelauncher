/**
 * @file init.c
 * @brief A test for inproper initialization.
 * @author Travis Lane
 * @version 0.1.0
 * @date 2014-01-11
 */

#include "../src/libmissilelauncher.h"

int main() {
  int result = 0;
  printf("********* Init Test *********\n");
  // First init
  result = ml_init_library();
  if(result != ML_OK) {
    fprintf(stderr, "Failed first initialization. Code: %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }
  // Second init, should fail
  result = ml_init_library();
  if (result == ML_OK) {
    fprintf(stderr, "Second initialization was successful when it should have failed. Code %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }
  printf("********* Test OK ********\n");
  return 0;
}
