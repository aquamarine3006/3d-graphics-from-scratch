#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <SDL3/SDL.h>

#include "vector_t.h"
#include "phase_t.h"
#include "model-nefertiti.h"

#define WINDOW_WIDTH 650U
#define WINDOW_HEIGHT 650U
#define FPS 120U
#define NEAR_ZERO 0.001f
#define POINT_SIZE 0.1f
#define POINT_OFFSET (POINT_SIZE / 2.0f)
#define STARTING_Z_POS 1.33f
#define ROTATIONAL_SPEED_PERCENTAGE 0.1f

#define USE_TRANSLATION_ANIMATION 0U //true
#define TRANSLATION_SPEED_PERCENTAGE 5.0f

#define BG_RED 100U
#define BG_GREEN 100U
#define BG_BLUE 180U
#define BG_A 255U

#define FG_RED 100U
#define FG_GREEN 0U
#define FG_BLUE 10U
#define FG_A 255U


typedef struct win_s {
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool is_running;
} win_t;

typedef enum {
	STATUS_OK = 0U,
	STATUS_ERR_ARG = 1U,
	STATUS_ERR_OP = 2U
} status_t;


static status_t init_window_renderer(win_t *restrict target)
{	
	if (NULL == target) {
		return STATUS_ERR_ARG;
	}
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Quit();
		return STATUS_ERR_ARG;	
	}


	target->window = SDL_CreateWindow("Plotter", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (NULL == target->window){
		SDL_Quit();
		return STATUS_ERR_ARG;
	}

	target->renderer = SDL_CreateRenderer(target->window, NULL);
	if (NULL == target->renderer) {
		SDL_DestroyWindow(target->window);
		SDL_Quit(); 
		return STATUS_ERR_ARG;
	}

	target->is_running = true;
	return STATUS_OK;
}

static float transform_x_coordinate(const float length)
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

static float transform_y_coordinate(const float length)
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

static float project_coordinate(const float coordinate, const float z)
{
	if (z < NEAR_ZERO) {
		return 0.0f;
	}

	return coordinate / z;
}

static void draw_point(win_t *restrict target, const float x, const float y, const float z)
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

static void draw_line(win_t *restrict target,
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

static void draw_points(win_t *restrict target, const float dz, const float angle) 
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

static void draw_lines(win_t *restrict target, const float dz, const float angle)
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

static void frame(win_t *restrict target, const float dz, const float angle)
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

static void mainloop(win_t *restrict target)
{
	if (NULL == target) {
		return;
	}

	SDL_Event event;

	float dz = STARTING_Z_POS;
	const float dt = 1.0f / (float)FPS;
	float angle = 0.0f;
	const float delay = 1000U / (float)FPS;
	const uint64_t delay_ns = (uint64_t)(delay * 1000000ULL);

	while (target->is_running) {
		while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) { 
					target->is_running = false; 
				}
		}

		#if USE_TRANSLATION_ANIMATION
			dz += 1.0f * dt * TRANSLATION_SPEED_PERCENTAGE;
		#endif
		angle += 2.0f * (float)M_PI * dt * ROTATIONAL_SPEED_PERCENTAGE;
		if (angle >= 2.0f * (float)M_PI) {
			angle -= 2.0f * (float)M_PI;
		}

		(void)frame(target, dz, angle);
		(void)SDL_RenderPresent(target->renderer);
		(void)SDL_DelayPrecise(delay_ns);
		
	}
}

int main(void)
{	
	win_t root = {
		.window = NULL,
		.renderer = NULL,
		.is_running = false
	};
	
	status_t win_status = init_window_renderer(&root);

	if (STATUS_ERR_ARG == win_status || 
	    STATUS_ERR_OP == win_status){
		return 1;
	}
	else if (STATUS_OK == win_status) {
		(void)mainloop(&root);
		(void)SDL_DestroyRenderer(root.renderer);
		(void)SDL_DestroyWindow(root.window);
		(void)SDL_Quit();
	}

	return 0;
}
