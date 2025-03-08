#include "game.h"

#include <SDL.h>

#include "choreo.h"
#include "render.h"
#include "util.h"

void
GameLoop(void)
{
	ActorWalkToPointLookDeg(A_PLAYER, 'A', 0.0f, 90.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'B', 0.0f, 45.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'C', 0.0f, 0.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'D', 0.0f, 45.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'E', 0.0f, 90.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'F', 0.0f, 45.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'G', 0.0f, 0.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'H', 0.0f, -45.0f);
	ActorWalkToPointLookDeg(A_PLAYER, 'I', 0.0f, -90.0f);
	
	ActorWalkToPoint(A_DUMMY, 'b');
	ActorWalkToPoint(A_DUMMY, 'a');
	ActorWalkToPoint(A_DUMMY, 'b');
	ActorWalkToPoint(A_DUMMY, 'a');
	ActorWalkToPoint(A_DUMMY, 'b');
	ActorWalkToPoint(A_DUMMY, 'a');
	ActorWalkToPoint(A_DUMMY, 'b');
	ActorWalkToPoint(A_DUMMY, 'a');
	
	for (;;)
	{
		BeginTick();
		
		// handle events.
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					OnWindowResize(e.window.data1, e.window.data2);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		// update.
		UpdateChoreo();
		
		// render.
		SetupFrameRender();
		RenderChoreo();
		PresentFrame();
		
		EndTick();
	}
}
