#include "render.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>

#define WND_TITLE "Fool Me Once, Shame on Me"

static SDL_Window *Wnd;
static SDL_GLContext GlContext;

i32
InitRender(void)
{
	// set up rendering data and structures.
	Wnd = SDL_CreateWindow(
		WND_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		0,
		0,
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
	);
	if (!Wnd)
	{
		ShowError("render: failed to create window - %s!", SDL_GetError());
		return 1;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	
	GlContext = SDL_GL_CreateContext(Wnd);
	if (!GlContext)
	{
		ShowError("render: failed to create GL context - %s!", SDL_GetError());
		return 1;
	}
	
	glewExperimental = GL_TRUE;
	GLenum Rc = glewInit();
	if (Rc != GLEW_OK)
	{
		ShowError("render: failed to init GLEW - %s!", glewGetErrorString(Rc));
		return 1;
	}
	
	return 0;
}

void
SetupFrameRender(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void
PresentFrame(void)
{
	SDL_GL_SwapWindow(Wnd);
}
