/**
 * @file ml_launcher.c
 * @brief 
 * @author Travis Lane
 * @version 0.0.2
 * @date 2014-01-02
 */

#include "libmissilelauncher_internal.h"

int16_t _ml_init_launcher(ml_launcher_t *launcher) {

  pthread_mutex_init(&(launcher->main_lock), NULL);
  return ML_OK;
}

int16_t _ml_cleanup_launcher(ml_launcher_t *launcher) {

  pthread_mutex_destroy(&(launcher->main_lock));
  return ML_OK;
}

int16_t ml_refrence_launcher(ml_launcher_t *launcher) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_NOT_IMPLEMENTED;
}


int16_t ml_derefrence_launcher(ml_launcher_t *launcher) {
  pthread_mutex_lock(&(launcher->main_lock));
  
  pthread_mutex_unlock(&(launcher->main_lock));
  return ML_NOT_IMPLEMENTED;
}
