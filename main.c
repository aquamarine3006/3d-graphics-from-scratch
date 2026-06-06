#include <stdint.h>
#include <math.h>

#include <SDL3/SDL.h>

#include "config.h"
#include "win.h"
#include "render.h"

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
