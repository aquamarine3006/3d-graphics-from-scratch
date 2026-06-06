#include "win.h"

#include <SDL3/SDL.h>

#include "config.h"

status_t init_window_renderer(win_t *restrict target)
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
