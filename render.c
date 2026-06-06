#include "render.h"

#include <stdint.h>

#include <SDL3/SDL.h>

#include "config.h"
#include "project.h"
#include "vector_t.h"
#include "phase_t.h"
#include "models/model_nefertiti.h"

void draw_point(win_t *restrict target, const float x, const float y, const float z)
{
	if (NULL == target) {
		return;
	}

	const float x_projection = project_coordinate(x,z);
	const float x_coordinate = transform_x_coordinate(x_projection) - POINT_OFFSET;

	const float y_projection = project_coordinate(y,z);
	const float y_coordinate = transform_y_coordinate(y_projection) - POINT_OFFSET;


	SDL_FRect point = { x_coordinate, y_coordinate, POINT_SIZE, POINT_SIZE};
	(void)SDL_RenderFillRect(target->renderer, &point);
}

void draw_line(win_t *restrict target,
		const float x1, const float y1, const float z1, 
		const float x2, const float y2, const float z2)
{
	if (NULL == target || NULL == target->renderer) {
		return;
	} 

	const float x1_projected = project_coordinate(x1, z1);
	const float y1_projected = project_coordinate(y1, z1);
	const float x2_projected = project_coordinate(x2, z2);
	const float y2_projected = project_coordinate(y2, z2);

	const float x1_transformed_corrected = transform_x_coordinate(x1_projected) - POINT_OFFSET;
	const float y1_transformed_corrected = transform_y_coordinate(y1_projected) - POINT_OFFSET;
	const float x2_transformed_corrected = transform_x_coordinate(x2_projected) - POINT_OFFSET;
	const float y2_transformed_corrected = transform_y_coordinate(y2_projected) - POINT_OFFSET;
	(void)SDL_RenderLine(target->renderer,
			     x1_transformed_corrected,
			     y1_transformed_corrected,
			     x2_transformed_corrected,
			     y2_transformed_corrected);
}

void draw_points(win_t *restrict target, const float dz, const float angle) 
{
	if (NULL == target || NULL == target->renderer) {
		return;
	}

	for (uint32_t r = 0; r < POINT_COUNT; r++) {
		const float x = points[r].x;
		const float y = points[r].y;
		const float z = points[r].z;
		
		const float x_rotated = rotate_x_around_yaxis(x, z, angle);
		const float z_rotated = rotate_z_around_yaxis(x, z, angle);

		const float z_translated_rotated = translate(z_rotated, dz);

		(void)draw_point(target, x_rotated, y, z_translated_rotated);
	}
}

void draw_lines(win_t *restrict target, const float dz, const float angle)
{
	if (NULL == target || NULL == target->renderer) {
		return;
	}

	for (uint32_t r = 0; r < PHASE_COUNT; r++) {
    		const uint32_t current_index = phases[r].index_from;
    		const uint32_t next_index = phases[r].index_to;
		
		if (current_index >= POINT_COUNT || next_index >= POINT_COUNT) {
			continue;
		}

		const float x_point1 = points[current_index].x;
		const float y_point1 = points[current_index].y;
		const float z_point1 = points[current_index].z;

		const float x_point2 = points[next_index].x;
		const float y_point2 = points[next_index].y;
		const float z_point2 = points[next_index].z;

		const float x_point1_rotated = rotate_x_around_yaxis(x_point1, z_point1, angle);
		const float z_point1_rotated = rotate_z_around_yaxis(x_point1, z_point1, angle);
		const float z_point1_rotated_translated = translate(z_point1_rotated, dz);

		const float x_point2_rotated = rotate_x_around_yaxis(x_point2, z_point2, angle);
		const float z_point2_rotated = rotate_z_around_yaxis(x_point2, z_point2, angle);
		const float z_point2_rotated_translated = translate(z_point2_rotated, dz);
    		

		(void)draw_line(target, 
			 x_point1_rotated, 
			 y_point1, 
			 z_point1_rotated_translated, 
			 x_point2_rotated, 
			 y_point2,
			 z_point2_rotated_translated);
	}
}

void frame(win_t *restrict target, const float dz, const float angle)
{
	if (NULL == target || NULL == target->renderer) {
		return;
	}

	(void)SDL_SetRenderDrawColor(target->renderer, BG_RED, BG_GREEN, BG_BLUE, BG_A);
	(void)SDL_RenderClear(target->renderer);
	(void)SDL_SetRenderDrawColor(target->renderer, FG_RED, FG_GREEN, FG_BLUE, FG_A);


	(void)draw_points(target, dz, angle);
	(void)draw_lines(target, dz, angle);
}
