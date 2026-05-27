#include<SDL3/SDL.h>
#include<stdbool.h>
#include<stdint.h>
#include<math.h>

#define WINDOW_WIDTH 650U
#define WINDOW_HEIGHT 650U
#define FPS 120

typedef struct{
	SDL_Window *pwindow;
	SDL_Renderer *prenderer;
	bool is_running;
} WIN;

bool CreateWindowRenderer(WIN *restrict target){
	
	if (NULL == target){
		return false;
	}
	if (!SDL_Init(SDL_INIT_VIDEO)){
		return false;	
	}
	target->pwindow = SDL_CreateWindow("Plotter", WINDOW_WIDTH, WINDOW_HEIGHT,0);
	if (NULL == target->pwindow){
		SDL_Quit();
		return false;
	}
	target->prenderer = SDL_CreateRenderer(target->pwindow, NULL);
	if (NULL == target->prenderer){
		SDL_DestroyWindow(target->pwindow);
		SDL_Quit();
		return false;
	}
	target->is_running = true;
	return true;
}

uint32_t TransformXCoordinate(float length){
	return (length + 1)/2*WINDOW_WIDTH;
}

uint32_t TransformYCoordinate(float length){
	return (1 - (length + 1)/2)*WINDOW_HEIGHT;
}

float ProjectX(float x, float z){
	return x/z;
}

float ProjectY(float y, float z){
	return y/z;
}
void DrawPoint(WIN *target, const float x, const float y, const float z){
	if (NULL == target){
		return;
	}
	const float size = 5;
	SDL_FRect point = { (TransformXCoordinate(ProjectX(x,z)) - size/2),
		(TransformYCoordinate(ProjectY(y,z)) - size/2),
		size, size};
	SDL_RenderFillRect(target->prenderer, &point);
}

float Translate(const float z, const float dz){
	return z + dz;
}

float RotateX_xz(const float x, const float z, const float angle){
	return x * cos(angle) - z * sin(angle);
}

float RotateZ_xz(const float x, const float z, const float angle){
	return x * sin(angle) + z * cos(angle);
}

void DrawLine(WIN *target, const float x1, const float y1, const float z1, const float x2, const float y2, const float z2){
	SDL_RenderLine(target->prenderer,
			TransformXCoordinate(ProjectX(x1, z1)),
			TransformYCoordinate(ProjectY(y1, z1)),
			TransformXCoordinate(ProjectX(x2, z2)),
			TransformYCoordinate(ProjectY(y2, z2)));
}

void frame(WIN *target, uint32_t rows, uint32_t cols, const float points[rows][cols], uint32_t rows_phase, uint32_t cols_phase, const uint32_t phases[rows_phase][cols_phase], const float dz, const float angle){
	SDL_SetRenderDrawColor(target->prenderer, 255U, 100U, 255U, 150U);
	SDL_RenderClear(target->prenderer);
	SDL_SetRenderDrawColor(target->prenderer, 99U, 35U, 188U, 255U);
	for (uint8_t r=0; r<rows; r++){
		float x = points[r][0];
		//float y = points[r][1];
		float z = points[r][2];
		DrawPoint(target, RotateX_xz(x,z,angle), points[r][1], Translate(RotateZ_xz(x,z,angle), dz));
	}
	for (uint32_t r = 0; r < rows_phase; r++) {
    		uint32_t a = phases[r][0];
    		uint32_t b = phases[r][1];
    		float ax = RotateX_xz(points[a][0], points[a][2], angle);
    		float az = Translate(RotateZ_xz(points[a][0], points[a][2], angle), dz);
    		float bx = RotateX_xz(points[b][0], points[b][2], angle);
    		float bz = Translate(RotateZ_xz(points[b][0], points[b][2], angle), dz);
    		DrawLine(target, ax, points[a][1], az, bx, points[b][1], bz);
	}
}

void mainloop(WIN *root){
	if (NULL == root){
		return;
	}
	SDL_Event event;

	float dz = 1;
	const float dt = 1.0f/FPS;
	float angle = 0;
	const int8_t delay = 1000/FPS;
	SDL_SetRenderDrawColor(root->prenderer, 255U, 100U, 255U, 150U);
	//float points[8][3] = {
	//	{0.25,0.25,0.25},
	//	{-0.25,0.25,0.25},
	//	{0.25,-0.25,0.25},
	//	{-0.25,-0.25,0.25},
	//	
	//	{0.25,0.25,-0.25},
	//	{-0.25,0.25,-0.25},
	//	{0.25,-0.25,-0.25},
	//	{-0.25,-0.25,-0.25}
	//};
	const float points[20][3] = {
	{0.202073f, 0.202073f, 0.202073f},
	{0.202073f, 0.202073f, -0.202073f},
	{0.202073f, -0.202073f, 0.202073f},
	{0.202073f, -0.202073f, -0.202073f},
	{-0.202073f, 0.202073f, 0.202073f},
	{-0.202073f, 0.202073f, -0.202073f},
	{-0.202073f, -0.202073f, 0.202073f},
	{-0.202073f, -0.202073f, -0.202073f},
	{0.000000f, 0.326960f, 0.124888f},
	{0.000000f, 0.326960f, -0.124888f},
	{0.000000f, -0.326960f, 0.124888f},
	{0.000000f, -0.326960f, -0.124888f},
	{0.124888f, 0.000000f, 0.326960f},
	{0.124888f, 0.000000f, -0.326960f},
	{-0.124888f, 0.000000f, 0.326960f},
	{-0.124888f, 0.000000f, -0.326960f},
	{0.326960f, 0.124888f, 0.000000f},
	{0.326960f, -0.124888f, 0.000000f},
	{-0.326960f, 0.124888f, 0.000000f},
	{-0.326960f, -0.124888f, 0.000000f},
};
	//uint32_t vertexes[12][2] = {
	//	{0,1},{2,3},{0,2},
	//	{1,3},{4,5},{6,7},
	//	{4,6},{5,7},{0,4},
	//	{1,5},{2,6},{3,7}
	//};
const uint32_t vertexes[30][2] = {
	{0U, 16U},
	{1U, 16U},
	{2U, 17U},
	{3U, 17U},
	{4U, 18U},
	{5U, 18U},
	{6U, 19U},
	{7U, 19U},
	{8U, 9U},
	{10U, 11U},
	{12U, 14U},
	{13U, 15U},
	{16U, 17U},
	{18U, 19U},
	{0U, 8U},
	{0U, 12U},
	{1U, 9U},
	{1U, 13U},
	{2U, 10U},
	{2U, 12U},
	{3U, 11U},
	{3U, 13U},
	{4U, 8U},
	{4U, 14U},
	{5U, 9U},
	{5U, 15U},
	{6U, 10U},
	{6U, 14U},
	{7U, 11U},
	{7U, 15U},
};
	bool flag = false;
	while (root->is_running){
		while (SDL_PollEvent(&event)){
				if (event.type == SDL_EVENT_QUIT) { root->is_running = false; }
				}
		if (flag){
			dz -= 1 * dt;
			angle += 2 * 3.1415 * dt * 2;
			if (dz <= 0.65f){
				dz = 0.65f;
				flag = false;
			}
		}
		else if (dz < 5.0){
			dz += 1 * dt;
			angle += 2 * 3.1415 * dt/2;
		}
		else {
			flag = true;
		}
		frame(root, 20, 3, points, 30, 2, vertexes, dz, angle);
		SDL_RenderPresent(root->prenderer);
		SDL_Delay(delay);
		}
}

int main(void){
	
	WIN root = {
		.pwindow = NULL,
		.prenderer = NULL,
		.is_running = false
	};
	if (!CreateWindowRenderer(&root)){
		return 1;
	}
	mainloop(&root);
	SDL_DestroyRenderer(root.prenderer);
	SDL_DestroyWindow(root.pwindow);
	SDL_Quit();
	return 0;
}
