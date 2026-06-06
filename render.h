#ifndef RENDER_H
#define RENDER_H

#include "win.h"

void draw_point(win_t *restrict target, const float x, const float y, const float z);
void draw_line(win_t *restrict target,
		const float x1, const float y1, const float z1, 
		const float x2, const float y2, const float z2);
void draw_points(win_t *restrict target, const float dz, const float angle);
void draw_lines(win_t *restrict target, const float dz, const float angle);
void frame(win_t *restrict target, const float dz, const float angle);

#endif
