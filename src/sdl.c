#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif
#include <string.h>
#include "lib.h"
#include "core.h"

int SCREEN_HEIGHT = 640;
int SCREEN_WIDTH = 480;
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
TTF_Font *font = NULL;
mm_session *session;
SDL_Table panel, state, control, play;
unsigned it_w, it_h, it2_w;

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
	if (TTF_Init() == -1) {
		printf("SDL_ttf cannont intialize! Erro: %s\n", TTF_GetError());
		exit(1);
	}
	// font = TTF_OpenFont("extra/Anonymous Pro B.ttf", 28);
	font = TTF_OpenFont("share/fonts/ProFont_r400-29.pcf", 29);
	if (font == NULL) {
		printf("Failed to load font! Error: %s\n", TTF_GetError());
		exit(1);
	}
	// surf = SDL_GetWindowSurface(win);
}
void clean()
{
	if (session) {
		mm_session_exit(session);
	}
	TTF_CloseFont(font);
	SDL_DestroyRenderer(rend);
	// SDL_FreeSurface(surf);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
}
SDL_Texture *sdl_print(char *s, int x, int y)
{
	SDL_Texture *tex;
	SDL_Rect rect;
	SDL_Surface *tmp = TTF_RenderUTF8_Solid(font, s, (SDL_Color){0, 0, 0});
	if (tmp == NULL) {
		printf("Unable to load font! Error: %s\n", TTF_GetError());
		clean();
		exit(1);
	}
	tex = SDL_CreateTextureFromSurface(rend, tmp);
	if (tex == NULL) {
		printf("Unable to create texture! Error: %s\n", SDL_GetError());
		clean();
		exit(1);
	}
	SDL_SetTextureColorMod(tex, 0, 0, 0);
	rect = (SDL_Rect){x - tmp->w / 2, y, tmp->w, tmp->h};
	SDL_RenderCopyEx(rend, tex, NULL, &rect, 0, 0, 0);
	SDL_FreeSurface(tmp);
	SDL_DestroyTexture(tex);
	return tex;
}
int setBg()
{
	SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(rend, NULL);
	return 0;
}
void initTables()
{
	it_w = SCREEN_WIDTH / (session->config->holes + 4),
	it_h = SCREEN_HEIGHT / (session->config->guesses + 4);
	it2_w = (SCREEN_WIDTH - it_w) / 9;
	panel = (SDL_Table){.x = it_w / 2,
			    .y = it_h / 2,
			    .w = it_w * session->config->holes,
			    .h = it_h * (session->config->guesses + 1),
			    .rows = session->config->guesses,
			    .cols = session->config->holes};
	state = (SDL_Table){.x = it_w / 2 + ((panel.cols + 1) * it_w),
			    .y = it_h / 2,
			    .w = it_w * 2,
			    .h = it_h * (session->config->guesses + 1),
			    .rows = session->config->guesses,
			    .cols = 2};
	control = (SDL_Table){.x = it_w / 2,
			      .y = SCREEN_HEIGHT - (it_h * 1.5F),
			      .w = it2_w * 3,
			      .h = it_h,
			      .rows = 1,
			      .cols = 3};
	play = (SDL_Table){.x = it_w / 2 + it2_w * 5,
			   .y = SCREEN_HEIGHT - (it_h * 1.5F),
			   .w = it2_w * 4,
			   .h = it_h,
			   .rows = 1,
			   .cols = 2};
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
void drawTableTop(SDL_Table *T)
{
	unsigned i;
	SDL_SetRenderDrawColor(rend, 0x00, 0x00, 0x00, 0x00);
	for (i = 0; i <= T->rows + 1; i++) {
		if (i == session->guessed + 1)
			continue;
		SDL_RenderDrawLine(rend, T->x, T->y + (it_h * i), T->x + T->w,
				   T->y + (it_h * i));
	}
	for (i = 0; i <= T->cols + 1; i++)
		SDL_RenderDrawLine(rend, T->x + (it_w * i), T->y,
				   T->x + (it_w * i), T->y + T->h);
	SDL_RenderPresent(rend);
}
int drawGuess(SDL_Table *T, SDL_Table *R, unsigned p)
{
	unsigned i;
	uint8_t *G = session->panel[p].combination;
	SDL_Rect rect;
	rect.h = T->h / ((T->rows + 1) * 3);
	rect.w = T->w / (T->cols * 3);
	rect.x = T->x + (T->w / (T->cols * 3));
	rect.y = T->y + ((it_h / 3) * (p * 3 + 1));
	for (i = 0; i < T->cols; i++) {
		SDL_SetRenderDrawColor(rend, 255 / (G[i] + 1),
				       (150 * G[i]) % 200, 100 / (G[i] % 3 + 1),
				       255);
		SDL_RenderFillRect(rend, &rect);
		rect.x += T->w / T->cols;
	}
	char s[2];
	sprintf(s, "%d", session->panel[p].inplace);
	sdl_print(s, R->x + R->w / 4, rect.y);
	sprintf(s, "%d",
		session->panel[p].insecret - session->panel[p].inplace);
	sdl_print(s, R->x + (R->w / 4) * 3, rect.y);
	SDL_RenderPresent(rend);
	return 0;
}
void redraw()
{
	unsigned i;
	setBg();
	drawTableTop(&panel);
	drawTableTop(&state);
	drawTable(&control);
	drawTable(&play);
	for (i = 0; i < session->guessed; i++)
		drawGuess(&panel, &state, i);
}
uint8_t *getGuess()
{
	SDL_Event event;
	uint8_t *str;
	unsigned i = 0;
	str = (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
	while (i < session->config->holes && SDL_WaitEvent(&event) > -1) {
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
			printf("Window Event: id: %d, event: %d\n",
			       event.window.windowID, event.window.event);
			if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
			    event.window.event == SDL_WINDOWEVENT_EXPOSED) {
				SDL_GetWindowSize(win, &SCREEN_WIDTH,
						  &SCREEN_HEIGHT);
				initTables(session);
				redraw(session);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			printf("MouseButtonEvent: button: %d, x= %d, y= %d\n",
			       event.button.button, event.button.x,
			       event.button.y);
			break;
		}
	}
	return str;
}
int main(int argc, char *argv[])
{
	uint8_t *g;
	init_sdl();
	session = mm_session_restore();
	if (session == NULL)
		;
	session = mm_session_new();
	initTables(session);
	for (;;) {
		redraw(session);
		while (session->state == MM_PLAYING ||
		       session->state == MM_NEW) {
			do {
				g = getGuess(session);
			} while (mm_play(session, g));
			redraw(session);
		}
		SDL_RenderClear(rend);
		mm_session_free(session);
		SDL_Delay(2000);
		session = mm_session_new();
		initTables(session);
	}
	clean();
	return 0;
}
