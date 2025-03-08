#ifndef RENDER_H
#define RENDER_H

#include <cglm/cglm.h>

#include "util.h"

enum Model
{
	M_PLANE = 0,
	M_CUBE,
	
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
	T_FLOOR,
	T_CEILING,
	T_WALL,
	
	T_END__
};

struct Camera
{
	vec3 Pos;
	f32 Pitch, Yaw;
};

extern struct Camera g_Camera;

i32 InitRender(void);
void SetupFrameRender(void);
void PresentFrame(void);
void OnWindowResize(i32 x, i32 y);
void SetShaderProgram(enum ShaderProgram Prog);
void RenderModel(enum Model m, enum Texture t, vec3 Pos, vec3 Rot, vec3 Scale);

#endif
