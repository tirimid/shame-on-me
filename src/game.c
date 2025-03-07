#include "game.h"

#include <SDL.h>

#include "render.h"
#include "util.h"

void
GameLoop(void)
{
	for (;;)
	{
		BeginTick();
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		SetupFrameRender();
		PresentFrame();
		
		EndTick();
	}
}
