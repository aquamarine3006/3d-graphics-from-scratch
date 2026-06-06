#include "project.h"

#include <math.h>

#include "config.h"

float transform_x_coordinate(const float length)
{
	if (!isfinite(length)) {
		return 0.0f;
	}
	
	const float transformed_len = (length + 1.0f) / 2.0f;
	float transformed_x_coordinate = transformed_len * (float)WINDOW_WIDTH;

	if (transformed_x_coordinate < 0.0f) {
		transformed_x_coordinate = 0.0f;
	}
	if (transformed_x_coordinate > (float)WINDOW_WIDTH) {
		transformed_x_coordinate = (float)WINDOW_WIDTH;
	}

	return transformed_x_coordinate;
}

float transform_y_coordinate(const float length)
{

	if (!isfinite(length)) {
		return 0.0f;
	}

	const float transformed_len = (length + 1.0f) / 2.0f;
	const float inverted_len = 1.0f - transformed_len;
	float transformed_y_coordinate = inverted_len * (float)WINDOW_HEIGHT;

	if (transformed_y_coordinate < 0.0f) {
		transformed_y_coordinate = 0.0f;
	}
	if (transformed_y_coordinate > (float)WINDOW_HEIGHT) {
		transformed_y_coordinate = (float)WINDOW_HEIGHT;
	}

	return transformed_y_coordinate;
}

float project_coordinate(const float coordinate, const float z)
{
	if (z < NEAR_ZERO) {
		return 0.0f;
	}

	return coordinate / z;
}
