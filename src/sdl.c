#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL.h>
#include <string.h>
#include "lib.h"
#include "core.h"

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 480
typedef struct {
	unsigned x;
	unsigned y;
	unsigned w;
	unsigned h;
	unsigned rows;
	unsigned cols;
} SDL_Table;

SDL_Window *win = NULL;
// SDL_Surface *surf = NULL;
SDL_Renderer *rend = NULL;
mm_session *session;

void init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialize! Error: %s\n",
		       SDL_GetError());
		exit(1);
	}
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
					SDL_WINDOW_SHOWN, &win, &rend)) {
		printf("Error on creating window and gettings renderer! Error: "
		       "%s\n",
		       SDL_GetError());
		exit(1);
	}
	// surf = SDL_GetWindowSurface(win);
}
void clean()
{
	if (session) {
		mm_session_exit(session);
	}
	SDL_DestroyRenderer(rend);
	// SDL_FreeSurface(surf);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
int setBg()
{
	SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(rend, NULL);
	return 0;
}
int drawTable(SDL_Table *T)
{
	unsigned i, h, w;
	h = T->h / T->rows;
	w = T->w / T->cols;
	SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
	for (i = 0; i <= T->rows; i++)
		SDL_RenderDrawLine(rend, T->x, T->y + (h * i), T->x + T->w,
				   T->y + (h * i));
	for (i = 0; i <= T->cols; i++)
		SDL_RenderDrawLine(rend, T->x + (w * i), T->y, T->x + (w * i),
				   T->y + T->h);
	SDL_RenderPresent(rend);
	return 0;
}
uint8_t *getGuess()
{
	SDL_Event event;
	uint8_t *str;
	unsigned i = 0;
	str = (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
	while (SDL_PollEvent(&event) > -1 && i < session->config->holes) {
		// SDL_PollEvent returns either 0 or 1
		switch (event.type) {
		case SDL_QUIT:
			clean();
			exit(0);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym >= SDLK_a &&
			    event.key.keysym.sym <=
				(session->config->colors + SDLK_a))
				str[i++] = event.key.keysym.sym - SDLK_a;
			printf("Key down event: %d (%c) \n",
			       event.key.keysym.sym, event.key.keysym.sym);
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				printf("Video resize event\n");
			break;
		default:
			// printf("Unknown event\n");
			// SDL_Delay(200);
			break;
		}
	}
	return str;
}
int drawGuess(SDL_Table *T, uint8_t *g, unsigned p)
{
	unsigned i;
	SDL_Rect rect;
	rect.h = T->h / (T->rows * 3);
	rect.w = T->w / (T->cols * 3);
	rect.y = T->y + ((T->h / (T->rows * 3)) * (p * 3 + 1));
	rect.x = T->x + (T->w / (T->cols * 3));
	for (i = 0; i < T->cols; i++) {
		SDL_SetRenderDrawColor(rend, 255 / (g[i] + 1),
				       (150 * g[i]) % 200, 100 / (g[i] % 3 + 1),
				       255);
		SDL_RenderFillRect(rend, &rect);
		rect.x += T->w / T->cols;
	}
	SDL_RenderPresent(rend);
	return 0;
}
int main(int argc, char *argv[])
{
	uint8_t *g;
	unsigned w, h;
	SDL_Table panel, state;
	init_sdl();
	session = mm_session_restore();
	if (session == NULL)
		session = mm_session_new(NULL);
	for (;;) {
		setBg();
		w = SCREEN_WIDTH / (session->config->holes + 4),
		h = SCREEN_HEIGHT / (session->config->guesses + 1);
		panel = (SDL_Table){.x = w / 2,
				    .y = h / 2,
				    .w = w * session->config->holes,
				    .h = h * session->config->guesses,
				    .rows = session->config->guesses,
				    .cols = session->config->holes};
		state = (SDL_Table){.x = w / 2 + ((panel.cols + 1) * w),
				    .y = h / 2,
				    .w = w * 2,
				    .h = h * session->config->guesses,
				    .rows = session->config->guesses,
				    .cols = 2};
		drawTable(&panel);
		drawTable(&state);
		while (session->state == MM_PLAYING ||
		       session->state == MM_NEW) {
			do {
				g = getGuess(session);
			} while (mm_play(session, g));
			drawGuess(&panel, g, session->guessed - 1);
		}
		SDL_RenderClear(rend);
		mm_session_free(session);
		session = mm_session_new(NULL);
		SDL_Delay(2000);
	}
	clean();
	return 0;
}
