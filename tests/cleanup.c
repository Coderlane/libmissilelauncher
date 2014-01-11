/**
 * @file init.c
 * @brief A test for inproper cleanup.
 * @author Travis Lane
 * @version 0.1.0
 * @date 2014-01-11
 */

#include "../src/libmissilelauncher.h"

int main() {
  int result = 0;
  printf("********* Cleanup Test *********\n");

  // Library isn't setup try cleaning it
  result = ml_cleanup_library();
  if(result == ML_OK) {
    fprintf(stderr, "First cleanup was successful when it should have failed. Code: %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }
  // Init the library
  result = ml_init_library();
  if(result != ML_OK) {
    fprintf(stderr, "Failed initialization. Code: %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }
  printf("Cleaning up\n");
  // Cleanup the library
  result = ml_cleanup_library();
  if(result != ML_OK) {
    fprintf(stderr, "Failed first proper cleanup. Code: %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }

  // Try cleaning up again
  result = ml_cleanup_library();
  if (result == ML_OK) {
    fprintf(stderr, "Second initialization was successful when it should have failed. Code %d\n", result);
    printf("********* Test FAILED ********\n");
    return -1;
  }
  printf("********* Test OK ********\n");
  return 0;
}
