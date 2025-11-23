#include "pid_controller.h"

float update_pid(pid_controller_t *controller, float error, float delta) {
  float proportional = controller->kp * error;
  controller->integral += controller->ki * error * delta;
  float derivative = controller->kd * (error - controller->last_error) / delta;

  controller->last_error = error;

  return proportional + controller->integral + derivative;
}
