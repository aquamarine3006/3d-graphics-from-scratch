#ifndef WIN_H
#define WIN_H

#include <stdbool.h>

#include <SDL3/SDL.h>

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

status_t init_window_renderer(win_t *restrict target);

#endif
