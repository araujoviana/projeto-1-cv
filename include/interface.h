#ifndef INTERFACE_H
#define INTERFACE_H

//=================================================================================================

#include <stdbool.h>
#include "window.h"
#include "image.h"
#include <SDL3_ttf/SDL_ttf.h>

//=================================================================================================

typedef struct
{
  Window window;
  Image image;
  char title[64];
} MainWindow;

typedef struct
{
  Window window;
  unsigned int histogram[256];
  float mean;
  float std_dev;
  SDL_FRect btn1_rect;
  SDL_FRect btn2_rect;
  TTF_Font *font;
  char title[64];
} HistogramWindow;

//=================================================================================================

// Inicialização Global SDL
bool Interface_init_sdl(void);
void Interface_quit_sdl(void);

// Funções da Janela Principal
bool MainWindow_initialize(MainWindow *mw, const char *title);
bool MainWindow_load_image(MainWindow *mw, const char *filename);
bool MainWindow_convert_image(MainWindow *mw);
void MainWindow_render(MainWindow *mw);
void MainWindow_shutdown(MainWindow *mw);
void MainWindow_update_mouse_title(MainWindow *mw, float mouse_x, float mouse_y);

// Funções da Janela do Histograma
bool HistogramWindow_initialize(HistogramWindow *hw, const char *title);
void HistogramWindow_update_statistics(HistogramWindow *hw, MainWindow *mw);
void HistogramWindow_render(HistogramWindow *hw);
void HistogramWindow_shutdown(HistogramWindow *hw);

//=================================================================================================

#endif