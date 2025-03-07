#ifndef RENDER_H
#define RENDER_H

#include "util.h"

enum Model
{
	M_PLANE = 0,
	
	M_END__
};

enum ShaderProgram
{
	SP_BASE = 0,
	
	SP_END__
};

enum Texture
{
	T_SOMETHING = 0,
	
	T_END__
};

i32 InitRender(void);
void SetupFrameRender(void);
void PresentFrame(void);
void OnWindowResize(i32 x, i32 y);

#endif
