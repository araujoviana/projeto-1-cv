#ifndef WINDOW_H
#define WINDOW_H

//=================================================================================================

#include <SDL3/SDL.h>
#include <stdbool.h>

//=================================================================================================

typedef struct Window Window;
struct Window
{
	SDL_Window *window;
	SDL_Renderer *renderer;
};

//=================================================================================================

bool Window_initialize(Window *window, const char *title, int width, int height, SDL_WindowFlags window_flags);

void Window_destroy(Window *window);

//=================================================================================================

#endif