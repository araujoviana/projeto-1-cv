//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "event.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// Helper
//------------------------------------------------------------------------------

static bool is_point_in_rect(float x, float y, const SDL_FRect *rect)
{
  return (x >= rect->x && x <= rect->x + rect->w && y >= rect->y && y <= rect->y + rect->h);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Event_loop(MainWindow *mw, HistogramWindow *hw)
{
  SDL_Log(">>> Event_loop()");

  MainWindow_render(mw);
  HistogramWindow_render(hw);

  SDL_Event event;
  bool isRunning = true;

  SDL_WindowID mw_id = SDL_GetWindowID(mw->window.window);
  SDL_WindowID hw_id = SDL_GetWindowID(hw->window.window);

  while (isRunning)
  {
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        isRunning = false;
        break;

      case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        isRunning = false;
        break;

      case SDL_EVENT_MOUSE_MOTION:
        if (event.motion.windowID == mw_id)
        {
          MainWindow_update_mouse_title(mw, event.motion.x, event.motion.y);
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.windowID == hw_id)
        {
          float x = event.button.x;
          float y = event.button.y;
          if (is_point_in_rect(x, y, &hw->btn1_rect))
          {
            SDL_Log("[Ação] Botão 1 clicado no Histograma!");
          }
          else if (is_point_in_rect(x, y, &hw->btn2_rect))
          {
            SDL_Log("[Ação] Botão 2 clicado no Histograma!");
          }
        }
        break;
      }
    }
  }
  SDL_Log("<<< Event_loop()");
}