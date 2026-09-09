//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "interface.h"
#include "window.h"
#include "image.h"
#include <stdio.h>

//------------------------------------------------------------------------------
// Tipos customizados, structs, constants, etc.
//------------------------------------------------------------------------------

enum constants
{
	DEFAULT_WINDOW_WIDTH = 640,
	DEFAULT_WINDOW_HEIGHT = 480,
	DEFAULT_WINDOW_TITLE_MAX_LENGTH = 64,
};


//------------------------------------------------------------------------------
// Variáveis globais
//------------------------------------------------------------------------------

const char* WINDOW_TITLE = "Itens 1 e 2 - Etapa 2";

static Window g_window = 
{ 
	.window = NULL, 
	.renderer = NULL 
};

static Image g_image = 
{
	.surface = NULL,
	.texture = NULL,
	.rect = { .x = 0.0f, .y = 0.0f, .w = DEFAULT_WINDOW_WIDTH, .h = DEFAULT_WINDOW_HEIGHT }
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------


void Interface_shutdown(void)
{
	SDL_Log(">>> shutdown()");

	Image_destroy(&g_image);
	Window_destroy(&g_window);

	SDL_Log("\tEncerrando SDL...");
	SDL_Quit();

	SDL_Log("<<< shutdown()");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

bool Interface_initialize(void)
{
	SDL_Log(">>> initialize()");

	SDL_Log("\tIniciando SDL...");
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("\t*** Erro ao iniciar a SDL: %s", SDL_GetError());
		SDL_Log("<<< initialize()");
		return false;
	}

	SDL_Log("\tCriando janela e renderizador...");
	if (!Window_initialize(&g_window, WINDOW_TITLE, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0))
	{
		SDL_Log("\t*** Erro ao criar a janela e/ou renderizador: %s", SDL_GetError());
		SDL_Log("<<< initialize()");
		return false;
	}

	SDL_Log("<<< initialize()");
	return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Interface_render(void)
{
	SDL_SetRenderDrawColor(g_window.renderer, 128, 128, 128, 255);
	SDL_RenderClear(g_window.renderer);

	SDL_RenderTexture(g_window.renderer, g_image.texture, NULL, &g_image.rect);

	SDL_RenderPresent(g_window.renderer);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

bool Interface_load_image(const char* filename)
{
    if(!Image_load(filename, g_window.renderer, &g_image))
	{
		return false;
	}
	
	Image_set_bounds(&g_image, 0.0f, 0.0f, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	
	return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

bool Interface_convert_image(void)
{
    return Image_convert(&g_image, g_window.renderer);
	Interface_render();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Interface_update_mouse_title(float mouse_x, float mouse_y) {
    char windowTitle[DEFAULT_WINDOW_TITLE_MAX_LENGTH] = { 0 };
    snprintf(windowTitle, DEFAULT_WINDOW_TITLE_MAX_LENGTH, "%s (%.0f, %.0f)", WINDOW_TITLE, mouse_x, mouse_y);
    SDL_SetWindowTitle(g_window.window, windowTitle);
}