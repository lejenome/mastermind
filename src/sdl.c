#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL.h>

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 480

int setSurfBg(SDL_Surface *surf)
{
	SDL_Surface *img = NULL, *tmp = NULL;
	SDL_Rect rect;
	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
	img = SDL_LoadBMP("test.bmp");
	if (img == NULL) {
		printf("SDL could not load bmp image 'test.bmp'! Error: %s\n",
		       SDL_GetError());
		return 1;
	}
	tmp = SDL_ConvertSurface(img, surf->format, 0);
	if (tmp == NULL) {
		printf("SDL could not optimaze the image! Error: %s\n",
		       SDL_GetError());
		SDL_FreeSurface(img);
		return 1;
	}
	SDL_FreeSurface(img);
	img = tmp;
	tmp = NULL;
	rect.x = 0;
	rect.y = 0;
	rect.h = SCREEN_HEIGHT;
	rect.w = SCREEN_WIDTH;
	// SDL_BlitSurface(img, NULL, surf, NULL);
	SDL_BlitScaled(img, NULL, surf, &rect);
	// SDL_UpdateWindowSurface(win);
	return 0;
}
int setBg(SDL_Renderer *rend)
{
	SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(rend, NULL);
	return 0;
}
int drawTable(SDL_Renderer *rend, unsigned rows, unsigned cols)
{
	unsigned w, h, x, y, i, H, W;
	w = SCREEN_WIDTH / (cols + 1);
	h = SCREEN_HEIGHT / (rows + 1);
	x = w / 2;
	y = h / 2;
	H = h * rows;
	W = w * cols;
	SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
	for (i = 0; i <= rows; i++)
		SDL_RenderDrawLine(rend, x, y + (h * i), x + W, y + (h * i));
	for (i = 0; i <= cols; i++)
		SDL_RenderDrawLine(rend, x + (w * i), y, x + (w * i), y + H);
	SDL_RenderPresent(rend);
	return 0;
}
int main(int argc, char *argv[])
{
	SDL_Window *win = NULL;
	SDL_Surface *surf = NULL;
	SDL_Renderer *rend = NULL;
	SDL_Event event;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialize! Error: %s\n",
		       SDL_GetError());
		exit(1);
	}
	win = SDL_CreateWindow("Master Mind", SDL_WINDOWPOS_UNDEFINED,
			       SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
			       SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		printf("SDL could not create win! Error: %s\n", SDL_GetError());
		exit(1);
	}
	surf = SDL_GetWindowSurface(win);
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (rend == NULL || surf == NULL) {
		printf(
		    "Error on getting window surface or renderer! Error %s\n",
		    SDL_GetError());
		return -1;
	}
	setBg(rend);
	drawTable(rend, 5, 6);
	while (SDL_PollEvent(&event) > -1) {
		// SDL_PollEvent returns either 0 or 1
		switch (event.type) {
		case SDL_QUIT:
			printf("Quit event\n");
			goto quit;
			break;
		case SDL_KEYDOWN:
			printf("Key down event: %d\n", event.key.keysym.sym);
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				printf("Video resize event\n");
			break;
		default:
			printf("Unknown event\n");
			SDL_Delay(200);
			break;
		}
	}
quit:
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
