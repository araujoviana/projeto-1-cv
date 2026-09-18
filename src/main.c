//------------------------------------------------------------------------------
// Projeto de Computação Visual
//
// Matheus Gabriel Viana Araujo   10420444
// Eduardo Takashi Missaka        10417877
// Arthur Meneses Neves           10425727
// João Victor Vidal Barbosa      10410165
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "interface.h"
#include "event.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

static MainWindow mw;
static HistogramWindow hw;

static void shutdown_app(void)
{
  MainWindow_shutdown(&mw);
  HistogramWindow_shutdown(&hw);
  Interface_quit_sdl();
}

int main(int argc, char *argv[])
{
  atexit(shutdown_app);

  if (argc != 2)
  {
    SDL_Log("Erro de chamada de programa: Forneca uma imagem como argumento.");
    return SDL_APP_FAILURE;
  }
  const char *IMAGE_FILENAME = argv[1];

  if (!Interface_init_sdl())
  {
    return SDL_APP_FAILURE;
  }

  if (!MainWindow_initialize(&mw, "Imagem Original"))
  {
    return SDL_APP_FAILURE;
  }

  if (!HistogramWindow_initialize(&hw, "Histograma e Analise"))
  {
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowParent(hw.window.window, mw.window.window);

  if (!MainWindow_load_image(&mw, IMAGE_FILENAME))
  {
    return SDL_APP_FAILURE;
  }

  if (!MainWindow_convert_image(&mw))
  {
    return SDL_APP_FAILURE;
  }

  HistogramWindow_update_statistics(&hw, &mw);

  Event_loop(&mw, &hw);

  return 0;
}
