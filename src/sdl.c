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

#define drawSecret()                                                           \
	drawCombination(session->secret->val, session->config->guesses, 0)
#define drawGuess(p) drawCombination(session->panel[p].combination, p, 1)

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
SDL_Renderer *rend = NULL;
TTF_Font *font = NULL, *icons = NULL;
;
mm_session *session = NULL;
uint8_t *curGuess = NULL; // combination of last guess combination
SDL_Table panel, state, control, play;
unsigned case_w, case_h, button_w;
SDL_Color *colors = NULL;

void init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initialize! Error: %s\n",
		       SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT,
					SDL_WINDOW_SHOWN, &win, &rend)) {
		printf("Error on creating window and gettings renderer! Error: "
		       "%s\n",
		       SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (TTF_Init() == -1) {
		printf("SDL_ttf cannont intialize! Erro: %s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}
	font = TTF_OpenFont("share/fonts/ProFont_r400-29.pcf", 28);
	icons = TTF_OpenFont("share/fonts/fontawesome-webfont.ttf", 31);
	if (font == NULL || icons == NULL) {
		printf("Failed to load font! Error: %s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}
	// surf = SDL_GetWindowSurface(win);
}
void clean()
{
	if (session) {
		mm_session_exit(session);
	}
	if (curGuess)
		free(curGuess);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(rend);
	// SDL_FreeSurface(surf);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
}
SDL_Texture *sdl_print_center(char *s, int x, int y, SDL_Color *color)
{
	SDL_Texture *tex;
	SDL_Rect rect;
	SDL_Surface *surf = TTF_RenderUTF8_Solid(
	    font, s, (color == NULL) ? (SDL_Color)fg_color : *color);
	if (surf == NULL) {
		printf("Unable to render font! Error: %s\n", TTF_GetError());
		clean();
		exit(EXIT_FAILURE);
	}
	tex = SDL_CreateTextureFromSurface(rend, surf);
	if (tex == NULL) {
		printf("Unable to create texture! Error: %s\n", SDL_GetError());
		clean();
		exit(EXIT_FAILURE);
	}
	rect = (SDL_Rect){x - surf->w / 2, y - surf->h / 2, surf->w, surf->h};
	SDL_RenderCopyEx(rend, tex, NULL, &rect, 0, 0, 0);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
	return tex;
}
SDL_Texture *sdl_print_icon(uint16_t c, int x, int y, SDL_Color *color)
{
	SDL_Texture *tex;
	SDL_Rect rect;
	SDL_Surface *surf = TTF_RenderGlyph_Blended(
	    icons, c, (color == NULL) ? (SDL_Color)fg_color : *color);
	if (surf == NULL) {
		printf("Unable to render font! Error: %s\n", TTF_GetError());
		clean();
		exit(EXIT_FAILURE);
	}
	tex = SDL_CreateTextureFromSurface(rend, surf);
	if (tex == NULL) {
		printf("Unable to create texture! Error: %s\n", SDL_GetError());
		clean();
		exit(EXIT_FAILURE);
	}
	rect = (SDL_Rect){x - surf->w / 2, y - surf->h / 2, surf->w, surf->h};
	SDL_RenderCopyEx(rend, tex, NULL, &rect, 0, 0, 0);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
	return tex;
}
int setBg()
{
	SDL_SetRenderDrawColor(rend, (SDL_Color)bg_color.r,
			       (SDL_Color)bg_color.g, (SDL_Color)bg_color.b,
			       (SDL_Color)bg_color.a);
	SDL_RenderFillRect(rend, NULL);
	return 0;
}
void initTables()
{
	case_w = SCREEN_WIDTH / (session->config->holes + 4);
	case_h = SCREEN_HEIGHT / (session->config->guesses + 4);
	button_w = (SCREEN_WIDTH - case_w) / 9;
	panel = (SDL_Table){.x = case_w / 2,
			    .y = case_h / 2,
			    .w = case_w * session->config->holes,
			    .h = case_h * (session->config->guesses + 1),
			    .rows = session->config->guesses,
			    .cols = session->config->holes};
	state = (SDL_Table){.x = case_w / 2 + ((panel.cols + 1) * case_w),
			    .y = case_h / 2,
			    .w = case_w * 2,
			    .h = panel.h,
			    .rows = session->config->guesses,
			    .cols = 2};
	control = (SDL_Table){.x = case_w / 2,
			      .y = SCREEN_HEIGHT - (case_h * 1.5F),
			      .w = button_w * 3,
			      .h = case_h,
			      .rows = 1,
			      .cols = 3};
	play = (SDL_Table){.x = case_w / 2 + button_w * 5,
			   .y = control.y,
			   .w = button_w * 4,
			   .h = case_h,
			   .rows = 1,
			   .cols = 2};
}
void initColors()
{
	unsigned i;
	if (colors)
		free(colors);
	colors =
	    (SDL_Color *)malloc(sizeof(SDL_Color) * session->config->colors);
	SDL_Color cl[] = {fg_red,  fg_green,   fg_yellow,
			  fg_blue, fg_magenta, fg_cyan};
	for (i = 0; i < session->config->colors && i < LEN(cl); i++)
		colors[i] = (SDL_Color){cl[i].r, cl[i].g, cl[i].b, cl[i].a};
	for (; i < session->config->colors; i++)
		colors[i] = (SDL_Color){255 / (i + 1), (150 * 2) % 200,
					100 / (i % 3 + 1), 255};
}
int drawTableBottom(SDL_Table *T)
{
	unsigned i, w;
	w = T->w / T->cols;
	for (i = 0; i <= T->rows; i++)
		SDL_RenderDrawLine(rend, T->x, T->y + (case_h * i), T->x + T->w,
				   T->y + (case_h * i));
	for (i = 0; i <= T->cols; i++)
		SDL_RenderDrawLine(rend, T->x + (w * i), T->y, T->x + (w * i),
				   T->y + T->h);
	return 0;
}
void drawTableTop(SDL_Table *T)
{
	unsigned i;
	for (i = 0; i <= T->rows + 1; i++) {
		if (i == session->guessed + 1 && session->state != MM_SUCCESS &&
		    session->state != MM_FAIL)
			continue;
		SDL_RenderDrawLine(rend, T->x, T->y + (case_h * i), T->x + T->w,
				   T->y + (case_h * i));
	}
	for (i = 0; i <= T->cols + 1; i++)
		SDL_RenderDrawLine(rend, T->x + (case_w * i), T->y,
				   T->x + (case_w * i), T->y + T->h);
}
void drawCombination(uint8_t *G, unsigned p, unsigned drawState)
{
	unsigned i;
	SDL_Rect rect;
	rect.h = case_h / 3;
	rect.w = case_w / 3;
	rect.x = panel.x + rect.w;
	rect.y = panel.y + case_h * p + case_h / 3;
	SDL_Color green, yellow;
	green = (SDL_Color)fg_green;
	yellow = (SDL_Color)fg_yellow;
	char c[2] = "a";
	for (i = 0; i < panel.cols; i++) {
		/*
		SDL_SetRenderDrawColor(rend, colors[G[i]].r, colors[G[i]].g,
				       colors[G[i]].b, colors[G[i]].a);
		SDL_RenderFillRect(rend, &rect);
		*/
		c[0] = 'a' + G[i];
		sdl_print_icon(0xF111, rect.x + rect.w / 2, rect.y + rect.h / 3,
			       colors + G[i]);
		// sdl_print_center(c, rect.x + rect.w / 2, rect.y + rect.h / 3,
		//		 NULL);
		rect.x += case_w;
	}
	if (drawState) {
		char s[2];
		sprintf(s, "%d", session->panel[p].inplace);
		sdl_print_center(s, state.x + state.w / 4, rect.y + rect.h / 3,
				 &green);
		sprintf(s, "%d",
			session->panel[p].insecret - session->panel[p].inplace);
		sdl_print_center(s, state.x + (state.w / 4) * 3,
				 rect.y + rect.h / 3, &yellow);
	}
}
void drawSelector()
{
	unsigned y, x, i;
	x = case_w;
	y = case_h * (session->guessed + 1);
	char c[2] = "a";
	for (i = 0; i < session->config->holes; i++) {
		sdl_print_icon(0xF0DE, x, y, NULL);
		c[0] = curGuess[i] + 'a';
		sdl_print_icon(0xF111, x, y + case_h / 2, &colors[c[0] - 'a']);
		// sdl_print_center(c, x, y + case_h / 2, NULL);
		sdl_print_icon(0xF0DD, x, y + case_h, NULL);
		x += case_w;
	}
}
void redraw()
{
	unsigned i;
	SDL_RenderClear(rend);
	setBg();
	SDL_SetRenderDrawColor(rend, (SDL_Color)br_color.r,
			       (SDL_Color)br_color.g, (SDL_Color)br_color.b,
			       (SDL_Color)br_color.a);
	drawTableTop(&panel);
	drawTableTop(&state);
	drawTableBottom(&control);
	drawTableBottom(&play);
	sdl_print_icon(0xF05A, control.x + button_w * 0.5,
		       control.y + case_h / 2, NULL);
	sdl_print_icon(0xF085, control.x + button_w * 1.5,
		       control.y + case_h / 2, NULL); // 0xF013
	sdl_print_icon(0xF097, control.x + button_w * 2.5,
		       control.y + case_h / 2, NULL);
	sdl_print_center("rest", play.x + button_w, play.y + case_h / 2, NULL);
	sdl_print_center("play", play.x + button_w * 3, play.y + case_h / 2,
			 NULL);
	for (i = 0; i < session->guessed; i++)
		drawGuess(i);
	if (session->state == MM_NEW || session->state == MM_PLAYING)
		drawSelector();
	else
		drawSecret();
	SDL_RenderPresent(rend);
}
unsigned onMouseUp(SDL_MouseButtonEvent e)
{
	unsigned i;
	if (e.x > panel.x && e.x < panel.x + panel.w &&
	    e.y > panel.y + case_h * session->guessed &&
	    e.y < panel.y + case_h * (session->guessed + 2)) {
		i = (e.x - panel.x) / case_w;
		if (e.y < panel.y + case_h * (session->guessed + 1)) {
			if (curGuess[i] < session->config->colors - 1)
				curGuess[i]++;
		} else {
			if (curGuess[i] > 0)
				curGuess[i]--;
		}
		redraw();
	}
	if (e.x > play.x && e.x < play.x + play.w && e.y > play.y &&
	    e.y < play.y + play.h)
		return (e.x < play.x + play.w / 2) ? 0 : 2;
	return 1;
}
uint8_t *getGuess(unsigned *play)
{
	SDL_Event event;
	uint8_t *str;
	unsigned i = 0;
	while (i < session->config->holes && SDL_WaitEvent(&event) > -1 &&
	       *play) {
		// SDL_PollEvent returns either 0 or 1
		switch (event.type) {
		case SDL_QUIT:
			clean();
			exit(EXIT_SUCCESS);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym >= SDLK_a &&
			    event.key.keysym.sym <
				(session->config->colors + SDLK_a)) {
				curGuess[i++] = event.key.keysym.sym - SDLK_a;
				redraw();
			}
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
				initTables();
				redraw();
			}
			break;
		case SDL_MOUSEBUTTONUP:
			printf("MouseButtonEvent: button: %d, x= %d, y= %d\n",
			       event.button.button, event.button.x,
			       event.button.y);
			*play = onMouseUp(event.button);
			if (*play == 2)
				goto done;
			break;
		}
	}
done:
	str = (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
	for (i = 0; i < session->config->holes; i++)
		str[i] = curGuess[i];
	return str;
}
int main(int argc, char *argv[])
{
	uint8_t *g;
	unsigned i, play;
	init_sdl();
	session = mm_session_restore();
	if (session == NULL)
		session = mm_session_new();
	initTables();
	initColors();
	for (;;) {
		curGuess =
		    (uint8_t *)malloc(sizeof(uint8_t) * session->config->holes);
		for (i = 0; i < session->config->holes; i++)
			curGuess[i] = 0;
		redraw();
		play = 1;
		while ((session->state == MM_PLAYING ||
			session->state == MM_NEW) &&
		       play) {
			do {
				play = 1;
				g = getGuess(&play);
			} while (mm_play(session, g) && play);
			redraw();
		}
		while (play && getGuess(&play))
			;
		SDL_RenderClear(rend);
		mm_session_free(session);
		free(curGuess);
		curGuess = NULL;
		session = mm_session_new();
		initTables();
		initColors();
	}
	clean();
	exit(EXIT_SUCCESS);
}
