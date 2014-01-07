/**
 * @file zeroing.c
 * @brief A breif demo of zeroing the device
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-07
 */

#include "../src/libmissilelauncher.h"

int main() {
  ml_launcher_t **launchers = NULL;
  uint32_t launchers_count = 0;
  int16_t failed;
  char input = '\0';
  
  ml_init_library();
  
  while(launchers_count == 0) {
    printf("Connect your launcher and press any key but q to check for it\n");
    printf("Press q to exit\n");
    input = getchar(); //Ignore newline
    if(input == 'q') {
      // Done exit
      ml_cleanup_library();
      exit(EXIT_SUCCESS);
    }
    // Check for a device
    failed = ml_get_launcher_array(&launchers, &launchers_count);
    if(launchers_count == 0 || failed == ML_NO_LAUNCHERS) {
      fprintf(stderr, "No launchers found, are you sure it is connected?\n");
    } else if(failed != ML_OK) {
      // Status code wasn't good
      fprintf(stderr, "Failed to get launcher array. Exit code: %d\n", failed);
    } 
  }
  // Pretty print
  if(launchers_count == 1) {
    printf("Found %d launcher.\nZeroing launchers...\n", launchers_count);
  } else {
    printf("Found %d launchers.\nZeroing launchers...\n", launchers_count);
  }

  for(uint32_t i = 0; i < launchers_count; i++) {
    failed = ml_zero_launcher(launchers[i]);
    if (failed != ML_OK) {
      fprintf(stderr, "Failed to zero launcher. Exit code: %d\n", failed);
    }
  }
  
  printf("Done zeroing, cleaning up.\n");
  ml_cleanup_library();
  return 0;
}
