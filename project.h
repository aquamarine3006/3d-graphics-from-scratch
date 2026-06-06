#ifndef PROJECT_H
#define PROJECT_H

#include <math.h>

#include "config.h"

static inline float translate(const float z, const float dz)
{
	return z + dz;
}

static inline float rotate_x_around_yaxis(const float x, const float z, const float angle)
{
	return x * cosf(angle) - z * sinf(angle);
}

static inline float rotate_z_around_yaxis(const float x, const float z, const float angle)
{
	return x * sinf(angle) + z * cosf(angle);
}

float project_coordinate(const float coordinate, const float z);
float transform_x_coordinate(const float length);
float transform_y_coordinate(const float length);

#endif
