// SPDX-License-Identifier: MIT
// Projeto 1 - Computacao Visual (UPM FCI CC) - 2026.2

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

enum
{
    WINDOW_WIDTH = 800,
    WINDOW_HEIGHT = 600,
};

static const char *WINDOW_TITLE = "Projeto 1 - CV";

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Erro ao iniciar a SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                     &window, &renderer))
    {
        SDL_Log("Erro ao criar janela/renderizador: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 24, 24, 24, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
