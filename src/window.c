//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "window.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Window_destroy(Window *window)
{
	SDL_Log(">>> Window_destroy()");

	if (!window)
	{
		SDL_Log("\t*** Erro: Janela/renderizador inválidos (window == NULL).");
		SDL_Log("<<< Window_destroy()");
		return;
	}

	SDL_Log("\tDestruindo Window->renderer...");
	SDL_DestroyRenderer(window->renderer);
	window->renderer = NULL;

	SDL_Log("\tDestruindo Window->window...");
	SDL_DestroyWindow(window->window);
	window->window = NULL;

	SDL_Log("<<< Window_destroy()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

bool Window_initialize(Window *window, const char *title, int width, int height, SDL_WindowFlags window_flags)
{
	SDL_Log("\tWindow_initialize(%s, %d, %d)", title, width, height);

	if (!window)
	{
		SDL_Log("\t\t*** Erro: Janela/renderizador inválidos (window == NULL).");
		return false;
	}

	return SDL_CreateWindowAndRenderer(title, width, height, window_flags, &window->window, &window->renderer);
}