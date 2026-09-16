//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "interface.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Tipos customizados, structs, constants, etc.
//------------------------------------------------------------------------------

enum constants
{
  MAIN_WINDOW_WIDTH = 1024,
  MAIN_WINDOW_HEIGHT = 768,
  HISTOGRAM_WINDOW_WIDTH = 640,
  HISTOGRAM_WINDOW_HEIGHT = 480,
  DEFAULT_WINDOW_TITLE_MAX_LENGTH = 64,
};

//------------------------------------------------------------------------------
// SDL Global
//------------------------------------------------------------------------------

bool Interface_init_sdl(void)
{
  SDL_Log(">>> Interface_init_sdl()");
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("\t*** Erro ao iniciar a SDL: %s", SDL_GetError());
    return false;
  }
  if (!TTF_Init())
  {
    SDL_Log("\t*** Erro ao iniciar a SDL_ttf: %s", SDL_GetError());
    return false;
  }
  return true;
}

void Interface_quit_sdl(void)
{
  SDL_Log("\tEncerrando SDL e TTF...");
  TTF_Quit();
  SDL_Quit();
}

//------------------------------------------------------------------------------
// MainWindow Functions
//------------------------------------------------------------------------------

bool MainWindow_initialize(MainWindow *mw, const char *title)
{
  strncpy(mw->title, title, sizeof(mw->title) - 1);
  mw->title[sizeof(mw->title) - 1] = '\0';

  mw->image.surface = NULL;
  mw->image.texture = NULL;
  mw->image.rect.x = 0.0f;
  mw->image.rect.y = 0.0f;
  mw->image.rect.w = MAIN_WINDOW_WIDTH;
  mw->image.rect.h = MAIN_WINDOW_HEIGHT;
  mw->equalized = false;
  mw->resolution_is_original = false;

  if (!Window_initialize(&mw->window, title, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, 0))
  {
    return false;
  }

  SDL_SetWindowPosition(mw->window.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  return true;
}

bool MainWindow_load_image(MainWindow *mw, const char *filename)
{
  if (!Image_load(filename, mw->window.renderer, &mw->image))
  {
    return false;
  }
  Image_set_bounds(&mw->image, 0.0f, 0.0f, (float)MAIN_WINDOW_WIDTH, (float)MAIN_WINDOW_HEIGHT);
  return true;
}

bool MainWindow_convert_image(MainWindow *mw)
{
  bool result = Image_convert(&mw->image, mw->window.renderer);
  MainWindow_render(mw);
  return result;
}

void MainWindow_toggle_equalization(MainWindow *mw)
{
  if (!mw)
    return;

  if (mw->equalized)
  {
    Image_show_original(&mw->image, mw->window.renderer);
    mw->equalized = false;
  }
  else
  {
    Image_equalize(&mw->image, mw->window.renderer);
    mw->equalized = true;
  }
  MainWindow_render(mw);
}

void MainWindow_toggle_resolution(MainWindow *mw)
{
  if (!mw || !mw->window.window)
    return;

  int target_w = 0;
  int target_h = 0;

  if (mw->resolution_is_original)
  {
    target_w = MAIN_WINDOW_WIDTH;
    target_h = MAIN_WINDOW_HEIGHT;
    mw->resolution_is_original = false;
  }
  else
  {
    target_w = mw->image.surface ? mw->image.surface->w : MAIN_WINDOW_WIDTH;
    target_h = mw->image.surface ? mw->image.surface->h : MAIN_WINDOW_HEIGHT;
    mw->resolution_is_original = true;
  }

  SDL_SetWindowSize(mw->window.window, target_w, target_h);
  Image_set_bounds(&mw->image, 0.0f, 0.0f, (float)target_w, (float)target_h);

  SDL_Rect bounds = {0};
  SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
  if (display_id != 0 && SDL_GetDisplayBounds(display_id, &bounds))
  {
    if (target_w > bounds.w || target_h > bounds.h)
    {
      SDL_SetWindowPosition(mw->window.window, 0, 0);
    }
    else
    {
      SDL_SetWindowPosition(mw->window.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
  }
  else
  {
    SDL_SetWindowPosition(mw->window.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  }

  MainWindow_render(mw);
}

void MainWindow_render(MainWindow *mw)
{
  SDL_SetRenderDrawColor(mw->window.renderer, 128, 128, 128, 255);
  SDL_RenderClear(mw->window.renderer);
  SDL_RenderTexture(mw->window.renderer, mw->image.texture, NULL, &mw->image.rect);
  SDL_RenderPresent(mw->window.renderer);
}

void MainWindow_shutdown(MainWindow *mw)
{
  Image_destroy(&mw->image);
  Window_destroy(&mw->window);
}

void MainWindow_update_mouse_title(MainWindow *mw, float mouse_x, float mouse_y)
{
  char windowTitle[128] = {0};
  snprintf(windowTitle, sizeof(windowTitle), "%s (%.0f, %.0f)", mw->title, mouse_x, mouse_y);
  SDL_SetWindowTitle(mw->window.window, windowTitle);
}

//------------------------------------------------------------------------------
// HistogramWindow Functions
//------------------------------------------------------------------------------

static void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, float x, float y,
                        SDL_Color color)
{
  if (!font || !text)
    return;
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  if (!surface)
    return;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture)
  {
    SDL_FRect rect = {x, y, (float)surface->w, (float)surface->h};
    SDL_RenderTexture(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
  }
  SDL_DestroySurface(surface);
}

static void render_text_centered(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                                 const SDL_FRect *rect, SDL_Color color)
{
  if (!font || !text || !rect)
    return;
  SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
  if (!surface)
    return;
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture)
  {
    SDL_FRect dest = {rect->x + (rect->w - (float)surface->w) / 2.0f,
                      rect->y + (rect->h - (float)surface->h) / 2.0f, (float)surface->w,
                      (float)surface->h};
    SDL_RenderTexture(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
  }
  SDL_DestroySurface(surface);
}

bool HistogramWindow_initialize(HistogramWindow *hw, const char *title)
{
  strncpy(hw->title, title, sizeof(hw->title) - 1);
  hw->title[sizeof(hw->title) - 1] = '\0';

  // Clear statistics
  memset(hw->histogram, 0, sizeof(hw->histogram));
  hw->mean = 0.0f;
  hw->std_dev = 0.0f;
  hw->btn1_hover = false;
  hw->btn2_hover = false;

  // Define buttons bounds (bottom of window)
  float btn_w = 200.0f;
  float btn_h = 40.0f;
  float padding = 40.0f;

  hw->btn1_rect.w = btn_w;
  hw->btn1_rect.h = btn_h;
  hw->btn1_rect.x = padding;
  hw->btn1_rect.y = HISTOGRAM_WINDOW_HEIGHT - btn_h - padding;

  hw->btn2_rect.w = btn_w;
  hw->btn2_rect.h = btn_h;
  hw->btn2_rect.x = HISTOGRAM_WINDOW_WIDTH - btn_w - padding;
  hw->btn2_rect.y = HISTOGRAM_WINDOW_HEIGHT - btn_h - padding;

  if (!Window_initialize(&hw->window, title, HISTOGRAM_WINDOW_WIDTH, HISTOGRAM_WINDOW_HEIGHT, 0))
  {
    return false;
  }

  SDL_SetWindowPosition(hw->window.window, 0, 0);

  const char *base_path = SDL_GetBasePath();
  char font_path[512] = {0};
  if (base_path)
  {
    snprintf(font_path, sizeof(font_path), "%sassets/Arial.ttf", base_path);
  }

  const char *font_paths[] = {font_path,
                              "C:/Windows/Fonts/arial.ttf",
                              "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                              "/usr/share/fonts/TTF/DejaVuSans.ttf",
                              "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
                              "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                              NULL};
  hw->font = NULL;
  for (int i = 0; font_paths[i] != NULL; ++i)
  {
    hw->font = TTF_OpenFont(font_paths[i], 16);
    if (hw->font)
    {
      break;
    }
  }
  if (!hw->font)
  {
    SDL_Log("Aviso: Nao foi possivel carregar a fonte. Textos não serao exibidos.");
  }

  return true;
}

void HistogramWindow_update_statistics(HistogramWindow *hw, MainWindow *mw)
{
  Image_calculate_statistics(&mw->image, hw->histogram, &hw->mean, &hw->std_dev);
  HistogramWindow_render(hw, mw);
}

void HistogramWindow_render(HistogramWindow *hw, MainWindow *mw)
{
  SDL_Renderer *renderer = hw->window.renderer;

  // Background: Dark Gray
  SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
  SDL_RenderClear(renderer);

  // Draw Histogram
  // Find max value for scaling
  unsigned int max_val = 1;
  for (int i = 0; i < 256; ++i)
  {
    if (hw->histogram[i] > max_val)
      max_val = hw->histogram[i];
  }

  float hist_x_start = 50.0f;
  float hist_y_start = 300.0f; // bottom of histogram
  float hist_width = 540.0f;   // fits nicely
  float hist_height = 200.0f;
  float bar_width = hist_width / 256.0f;

  SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255); // Green bars
  for (int i = 0; i < 256; ++i)
  {
    float h = ((float)hw->histogram[i] / max_val) * hist_height;
    SDL_FRect bar = {hist_x_start + i * bar_width, hist_y_start - h, bar_width, h};
    SDL_RenderFillRect(renderer, &bar);
  }

  // Draw axes
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderLine(renderer, hist_x_start, hist_y_start, hist_x_start + hist_width,
                 hist_y_start); // X axis
  SDL_RenderLine(renderer, hist_x_start, hist_y_start, hist_x_start,
                 hist_y_start - hist_height); // Y axis

  // Draw buttons
  if (hw->btn1_hover)
  {
    SDL_SetRenderDrawColor(renderer, 100, 160, 220, 255); // Azul claro (hover)
  }
  else
  {
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // Azul neutro
  }
  SDL_RenderFillRect(renderer, &hw->btn1_rect);

  if (hw->btn2_hover)
  {
    SDL_SetRenderDrawColor(renderer, 100, 160, 220, 255); // Azul claro (hover)
  }
  else
  {
    SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // Azul neutro
  }
  SDL_RenderFillRect(renderer, &hw->btn2_rect);

  // Button borders
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_RenderRect(renderer, &hw->btn1_rect);
  SDL_RenderRect(renderer, &hw->btn2_rect);

  // Text rendering
  if (hw->font)
  {
    SDL_Color textColor = {255, 255, 255, 255};
    char statBuf[128];
    const char *brilho = (hw->mean < 85.0f) ? "Escura" : ((hw->mean <= 170.0f) ? "Média" : "Clara");
    const char *contraste =
        (hw->std_dev < 30.0f) ? "Baixo" : ((hw->std_dev <= 60.0f) ? "Médio" : "Alto");

    snprintf(statBuf, sizeof(statBuf), "Media de Intensidade: %.2f (Imagem %s)", hw->mean, brilho);
    render_text(renderer, hw->font, statBuf, 50.0f, 320.0f, textColor);

    snprintf(statBuf, sizeof(statBuf), "Desvio Padrao: %.2f (Contraste %s)", hw->std_dev,
             contraste);
    render_text(renderer, hw->font, statBuf, 50.0f, 350.0f, textColor);

    // Button text
    const char *btn1_text = (mw && mw->equalized) ? "Ver original" : "Equalizar";
    const char *btn2_text = (mw && mw->resolution_is_original) ? "1024x768" : "Resolução original";

    render_text_centered(renderer, hw->font, btn1_text, &hw->btn1_rect, textColor);
    render_text_centered(renderer, hw->font, btn2_text, &hw->btn2_rect, textColor);
  }

  SDL_RenderPresent(renderer);
}

void HistogramWindow_shutdown(HistogramWindow *hw)
{
  if (hw->font)
  {
    TTF_CloseFont(hw->font);
    hw->font = NULL;
  }
  Window_destroy(&hw->window);
}