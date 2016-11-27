/**
 * @file zeroing.c
 * @brief A breif demo of zeroing the device
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-07
 */

#include <stdio.h>
#include <stdlib.h>

#include <libmissilelauncher/libmissilelauncher.h>

int main()
{
  ml_launcher_t **launchers = NULL;
  uint32_t launchers_count = 0;
  ml_error_code rv;
  char input = '\0';

  // Always initialize the library before using any functions.
  // Failure to do so will result it unexpected behavior.
  ml_library_init();

  while(launchers_count == 0) {
    printf("Connect your launcher now. "
           "Press q to exit or any other key to scan\n");
    // Note the poll rate sets how often to check for a new device.
    input = getchar(); //Ignore newline
    if(input == 'q') {
      // Done, exit
      ml_library_cleanup();
      exit(EXIT_SUCCESS);
    }

    // Get a new launcher array and see if anything was detected
    rv = ml_launcher_array_new(&launchers, &launchers_count);
    // Check exit code
    if(launchers_count == 0 || rv == ML_NO_LAUNCHERS) {
      // No launchers, note there are two ways to detect this state
      fprintf(stderr, "No launchers found, are you sure it is connected?\n");
    } else if(rv != ML_OK) {
      // Status code wasn't good
      fprintf(stderr, "Failed to get launcher array. "
              "Exit code: %d\n", rv);
    }
  }
  // Pretty print
  if(launchers_count == 1) {
    printf("Found %d launcher.\nZeroing launchers...\n", launchers_count);
  } else {
    printf("Found %d launchers.\nZeroing launchers...\n", launchers_count);
  }
  // Iterates over each launcher and attempts to zero it.
  for(uint32_t i = 0; i < launchers_count; i++) {
    // Zero current launcher
    rv = ml_launcher_claim(launchers[i]);
    if(rv != ML_OK) {
      fprintf(stderr, "Failed to claim launcher. "
              "Exit code: %d\n", rv);
			continue;
    }

    rv = ml_launcher_zero(launchers[i]);
    if (rv != ML_OK) {
      fprintf(stderr, "Failed to zero launcher. "
              "Exit code: %d\n", rv);
    }

    rv = ml_launcher_unclaim(launchers[i]);
    if(rv != ML_OK) {
      fprintf(stderr, "Failed to claim launcher. "
              "Exit code: %d\n", rv);
    }
  }
  // Free the launcher array when you are done.
  rv = ml_launcher_array_free(launchers);
  if(rv != ML_OK) {
    fprintf(stderr, "Failed to free the launcher array. "
            "Exit code: %d\n", rv);
  }

  printf("Done zeroing, cleaning up.\n");
  // Clean up when you are done.
  ml_library_cleanup();
  return 0;
}
