#include "maths.h"

void normalize(vec3 *vec) {
  float l = 1.0 / length(vec);
  vec->x *= l;
  vec->y *= l;
  vec->z *= l;
}

float length(vec3 *vec) {
  return sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
}

float dot(vec3 *a, vec3 *b) { return a->x * b->x + a->y * b->y + a->z * a->z; }

void cross(vec3 *dest, vec3 *a, vec3 *b) {
  float x = a->y * b->z - a->z * b->y;
  float y = a->z * b->x - a->x * b->z;
  float z = a->x * b->y - a->y * b->x;
  dest->x = x;
  dest->y = y;
  dest->z = z;
}

void add(vec3 *dest, vec3 *a, vec3 *b) {
  dest->x = a->x + b->x;
  dest->y = a->y + b->y;
  dest->z = a->z + b->z;
}

void sub(vec3 *dest, vec3 *a, vec3 *b) {
  dest->x = a->x - b->x;
  dest->y = a->y - b->y;
  dest->z = a->z - b->z;
}
