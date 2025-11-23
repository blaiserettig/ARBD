#ifndef H_GYRO
#define H_GYRO

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "maths.h"

typedef struct {
  vec3 acc;
  vec3 rot;
} gyro_t;

gyro_t setup_gyro();
/**
 * Requests and updates values of gyro into the given gyro_t*
 */
void request_gyro_values(gyro_t *gyro);
/**
 * Deletes gyro I2C communication. Must use setup_gyro to create the gyro again.
 */
void disable_gyro();

#endif // !H_GYRO
