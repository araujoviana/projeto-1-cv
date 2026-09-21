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
  HistogramWindow_render(hw, mw);

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

      case SDL_EVENT_WINDOW_RESIZED:
        if (event.window.windowID == mw_id)
        {
          MainWindow_render(mw); // o tamanho so muda de fato apos o evento, redesenha com ele
        }
        break;

      case SDL_EVENT_KEY_DOWN:
        if (event.key.windowID == mw_id && event.key.key == SDLK_S && !event.key.repeat)
        {
          Image_save_current("output_image.png", (int)mw->image.rect.w, (int)mw->image.rect.h);
        }
        break;

      case SDL_EVENT_MOUSE_MOTION:
        if (event.motion.windowID == mw_id)
        {
          MainWindow_update_mouse_title(mw, event.motion.x, event.motion.y);
        }
        else if (event.motion.windowID == hw_id)
        {
          bool h1 = is_point_in_rect(event.motion.x, event.motion.y, &hw->btn1_rect);
          bool h2 = is_point_in_rect(event.motion.x, event.motion.y, &hw->btn2_rect);
          if (h1 != hw->btn1_hover || h2 != hw->btn2_hover)
          {
            hw->btn1_hover = h1;
            hw->btn2_hover = h2;
            HistogramWindow_render(hw, mw);
          }
        }
        break;

      case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        if (event.window.windowID == hw_id)
        {
          if (hw->btn1_hover || hw->btn2_hover)
          {
            hw->btn1_hover = false;
            hw->btn2_hover = false;
            HistogramWindow_render(hw, mw);
          }
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.windowID == hw_id && event.button.button == SDL_BUTTON_LEFT)
        {
          float x = event.button.x;
          float y = event.button.y;
          if (is_point_in_rect(x, y, &hw->btn1_rect))
          {
            SDL_Log("[Ação] Botão de equalização clicado!");
            hw->btn1_pressed = true;
            HistogramWindow_render(hw, mw);
            SDL_Delay(80); // deixa o estado "clicado" visivel antes de redesenhar
            hw->btn1_pressed = false;

            MainWindow_toggle_equalization(mw);
            HistogramWindow_update_statistics(hw, mw);
          }
          else if (is_point_in_rect(x, y, &hw->btn2_rect))
          {
            SDL_Log("[Ação] Botão de alternância de resolução clicado!");
            hw->btn2_pressed = true;
            HistogramWindow_render(hw, mw);
            SDL_Delay(80); // deixa o estado "clicado" visivel antes de redesenhar
            hw->btn2_pressed = false;

            MainWindow_toggle_resolution(mw);
            HistogramWindow_render(hw, mw);
          }
        }
        break;
      }
    }
  }
  SDL_Log("<<< Event_loop()");
}