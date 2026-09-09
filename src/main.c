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

int main(int argc, char *argv[]) {
	atexit(Interface_shutdown);
	
	if (argc != 2) 
	{
		SDL_Log("Erro de chamada de programa");
	}
	const char* IMAGE_FILENAME = argv[1];
	
	if (!Interface_initialize()) 
	{
		return SDL_APP_FAILURE;
	}
	
	if (!Interface_load_image(IMAGE_FILENAME)) 
	{
		return SDL_APP_FAILURE;
	}
	
	if(!Interface_convert_image())
	{
		return SDL_APP_FAILURE;
	}
	
	Event_loop();

	return 0;
}


