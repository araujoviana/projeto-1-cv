//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "event.h"
#include "interface.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void Event_loop(void)
{	
	SDL_Log(">>> Event_loop()");

	Interface_render();
	
	SDL_Event event;
	bool isRunning = true;
	
	while (isRunning)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_EVENT_QUIT:
					isRunning = false;
					break;
				case SDL_EVENT_MOUSE_MOTION:			
					Interface_update_mouse_title(event.motion.x, event.motion.y);
					break;
			}
		}
	}
	SDL_Log("<<< Event_loop()");
}