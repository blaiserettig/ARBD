#ifndef H_MATHS
#define H_MATHS

#include <math.h>

typedef struct {
  float x, y, z;
} vec3;

void normalize(vec3 *vec);
float length(vec3 *vec);
float dot(vec3 *a, vec3 *b);
void cross(vec3 *dest, vec3 *a, vec3 *b);
void add(vec3 *dest, vec3 *a, vec3 *b);
void sub(vec3 *dest, vec3 *a, vec3 *b);

#endif // !H_MATHS
