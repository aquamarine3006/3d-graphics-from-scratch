#include<stdbool.h>
#include<stdint.h>
#include<math.h>
#include<SDL3/SDL.h>
#include"model.h"

#define WINDOW_WIDTH 650U
#define WINDOW_HEIGHT 650U
#define FPS 120U
#define NEAR_ZERO 0.001f

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool is_running;
} win_t;

static bool init_window_renderer(win_t *restrict target)
{	
	if (NULL == target) {
		return false;
	}
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return false;	
	}


	target->window = SDL_CreateWindow("Plotter", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (NULL == target->window){
		SDL_Quit();
		return false;
	}

	target->renderer = SDL_CreateRenderer(target->window, NULL);
	if (NULL == target->renderer) {
		SDL_DestroyWindow(target->window);
		SDL_Quit(); 
		return false;
	}

	target->is_running = true;
	return true;
}

static float transform_x_coordinate(float length)
{
	float transformed_x_coordinate = (length + 1.0f) / 2.0f * (float)WINDOW_WIDTH;

	if (transformed_x_coordinate < 0.0f) {
		transformed_x_coordinate = 0.0f;
	}
	if (transformed_x_coordinate > (float)WINDOW_WIDTH) {
		transformed_x_coordinate = (float)WINDOW_WIDTH;
	}

	return (float)transformed_x_coordinate;
}

static float transform_y_coordinate(float length)
{
	float transformed_y_coordinate = (1 - (length + 1.0f) / 2.0f) * (float)WINDOW_HEIGHT;

	if (transformed_y_coordinate < 0.0f) {
		transformed_y_coordinate = 0.0f;
	}
	if (transformed_y_coordinate > (float)WINDOW_HEIGHT) {
		transformed_y_coordinate = (float)WINDOW_HEIGHT;
	}

	return (float)transformed_y_coordinate;
}

static float project_x(float x, float z)
{
	if (z < NEAR_ZERO) {
		return 0.0f;
	}

	return x / z;
}

static float project_y(float y, float z)
{
	if (z < NEAR_ZERO) {
		return 0.0f;
	}

	return y / z;
}

static void draw_point(win_t *target, const float x, const float y, const float z)
{
	if (NULL == target) {
		return;
	}

	const float size = 5.0f;
	const float offset = size / 2.0f;
	
	const float x_projection = project_x(x,z);
	const float x_coordinate = transform_x_coordinate(x_projection) - offset;

	const float y_projection = project_y(y,z);
	const float y_coordinate = transform_y_coordinate(y_projection) - offset;


	SDL_FRect point = { x_coordinate, y_coordinate, size, size};
	(void)SDL_RenderFillRect(target->renderer, &point);
}

static inline float translate(const float z, const float dz)
{
	return z + dz;
}

static inline float rotate_x_xz(const float x, const float z, const float angle)
{
	return x * cosf(angle) - z * sinf(angle);
}

static inline float rotate_z_xz(const float x, const float z, const float angle)
{
	return x * sinf(angle) + z * cosf(angle);
}

static void draw_line(win_t *target,
		const float x1, const float y1, const float z1, 
		const float x2, const float y2, const float z2)
{
	(void)SDL_RenderLine(target->renderer,
			transform_x_coordinate(project_x(x1, z1)),
			transform_y_coordinate(project_y(y1, z1)),
			transform_x_coordinate(project_x(x2, z2)),
			transform_y_coordinate(project_y(y2, z2)));
}

static void frame(win_t *target, const float dz, const float angle)
{
	(void)SDL_SetRenderDrawColor(target->renderer, 255U, 100U, 255U, 150U);
	(void)SDL_RenderClear(target->renderer);
	(void)SDL_SetRenderDrawColor(target->renderer, 99U, 35U, 188U, 255U);


	for (uint32_t r = 0; r < POINT_COUNT; r++) {
		const float x = points[r][0];
		const float y = points[r][1];
		const float z = points[r][2];
		
		float x_rotated = rotate_x_xz(x, z, angle);
		float z_rotated = rotate_z_xz(x, z, angle);

		float z_translated_rotated = translate(z_rotated, dz);

		(void)draw_point(target, x_rotated, y, z_translated_rotated);
	}

	for (uint32_t r = 0; r < PHASE_COUNT; r++) {
    		const uint32_t current_index = phases[r][0];
    		const uint32_t next_index = phases[r][1];

		const float x_point_one = points[current_index][0];
		const float y_point_one = points[current_index][1];
		const float z_point_one = points[current_index][2];

		const float x_point_two = points[next_index][0];
		const float y_point_two = points[next_index][1];
		const float z_point_two = points[next_index][2];

		const float x_point_one_rotated = rotate_x_xz(x_point_one, z_point_one, angle);
		const float z_point_one_rotated = rotate_z_xz(x_point_one, z_point_one, angle);
		const float z_point_one_rotated_translated = translate(z_point_one_rotated, dz);

		const float x_point_two_rotated = rotate_x_xz(x_point_two, z_point_two, angle);
		const float z_point_two_rotated = rotate_z_xz(x_point_two, z_point_two, angle);
		const float z_point_two_rotated_translated = translate(z_point_two_rotated, dz);
    		

		(void)draw_line(target, 
			 x_point_one_rotated, 
			 y_point_one, 
			 z_point_one_rotated_translated, 
			 x_point_two_rotated, 
			 y_point_two,
			 z_point_two_rotated_translated);
	}
}

static void mainloop(win_t *root)
{
	if (NULL == root) {
		return;
	}

	SDL_Event event;

	float dz = 1.0f;
	const float dt = 1.0f / FPS;
	float angle = 0.0f;
	const int32_t delay = 1000U / FPS;

	(void)SDL_SetRenderDrawColor(root->renderer, 255U, 100U, 255U, 150U);

	bool flag = false;
	while (root->is_running) {
		while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) { 
					root->is_running = false; 
				}
		}

		if (flag) {
			//dz -= 1 * dt * 0.5f;
			angle += 2.0 * (float)M_PI * dt / 10.0f;
			if (dz <= 0.01f){
				dz = 0.01f;
				flag = false;
			}
		}
		else if (dz < 8.0) {
			//dz += 1 * dt * 0.5f;
			angle += 2.0 * (float)M_PI * dt / 10.0f;
		}
		else {
			flag = true;
		}

		(void)frame(root, dz, angle);
		(void)SDL_RenderPresent(root->renderer);
		(void)SDL_Delay(delay);
		}
}

int main(void)
{	
	win_t root = {
		.window = NULL,
		.renderer = NULL,
		.is_running = false
	};

	if (!init_window_renderer(&root)){
		return 1;
	}

	(void)mainloop(&root);
	(void)SDL_DestroyRenderer(root.renderer);
	(void)SDL_DestroyWindow(root.window);
	(void)SDL_Quit();

	return 0;
}
