#ifndef H_PID_CONTROLLER
#define H_PID_CONTROLLER

/**
 * Initialize integral variable to 0!!!!
 */
typedef struct {
  float kp;
  float ki;
  float kd;
  float integral;
  float last_error;
} pid_controller_t;

/**
 * Updates the pid and returns the calculated next value.
 *
 * @param controller - pid to be used
 * @param error - the difference between the desired and actual value
 * @param delta - delta time between previous and current function call (or
 * frame time)
 */
float update_pid(pid_controller_t *controller, float error, float delta);

#endif // H_PID_CONTROLLER
