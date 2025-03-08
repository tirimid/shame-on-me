#include "render.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>

#include "util.h"

// raw render data.
#include "img/ceiling_png.h"
#include "img/floor_png.h"
#include "img/something_png.h"
#include "img/wall_png.h"
#include "model/cube_obj.h"
#include "model/plane_obj.h"
#include "shader/base_frag_glsl.h"
#include "shader/base_vert_glsl.h"

#define WND_TITLE "Shame on Me"
#define MAX_SHADER_LOG_LEN 512
#define CAM_UP_DIRECTION (vec3){0.0f, 1.0f, 0.0f}
#define CAM_FOV 70.0f
#define CAM_CLIP_NEAR 0.1f
#define CAM_CLIP_FAR 500.0f

#define INCLUDE_MODEL(Name) \
	{ \
		.VertData = Name##_obj_VertData, \
		.IdxData = Name##_obj_IdxData, \
		.VertCnt = Name##_obj_VERT_CNT, \
		.IdxCnt = Name##_obj_IDX_CNT \
	}

#define INCLUDE_SHADER_PROGRAM(VertName, FragName) \
	{ \
		.VertSrc = (char *)VertName##_glsl, \
		.FragSrc = (char *)FragName##_glsl, \
		.VertSrcLen = sizeof(VertName##_glsl), \
		.FragSrcLen = sizeof(FragName##_glsl) \
	}

#define INCLUDE_TEXTURE(Name) \
	{ \
		.Data = Name##_png, \
		.Size = sizeof(Name##_png) \
	}

struct ModelData
{
	f32 const *VertData;
	usize VertCnt;
	u32 const *IdxData;
	usize IdxCnt;
	
	u32 Vbo, Ibo, Vao;
};

struct ShaderProgramData
{
	char const *VertSrc, *FragSrc;
	i32 VertSrcLen, FragSrcLen;
	
	u32 Vert, Frag;
	u32 Prog;
};

struct TextureData
{
	u8 const *Data;
	usize Size;
	
	SDL_Surface *Surf;
	u32 Tex;
};

struct Camera g_Camera;

static void DeleteGlContext(void);

static SDL_Window *Wnd;
static SDL_GLContext GlContext;
static u32 i_ModelMat, i_ViewMat, i_ProjMat;

// data tables.
static struct ModelData ModelData[M_END__] =
{
	INCLUDE_MODEL(plane),
	INCLUDE_MODEL(cube)
};

static struct ShaderProgramData ShaderProgramData[SP_END__] =
{
	INCLUDE_SHADER_PROGRAM(base_vert, base_frag)
};

static struct TextureData TextureData[T_END__] =
{
	INCLUDE_TEXTURE(something),
	INCLUDE_TEXTURE(floor),
	INCLUDE_TEXTURE(ceiling),
	INCLUDE_TEXTURE(wall)
};

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
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
	);
	if (!Wnd)
	{
		ShowError("render: failed to create window - %s!", SDL_GetError());
		return 1;
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	
	GlContext = SDL_GL_CreateContext(Wnd);
	if (!GlContext)
	{
		ShowError("render: failed to create GL context - %s!", SDL_GetError());
		return 1;
	}
	atexit(DeleteGlContext);
	
	glewExperimental = GL_TRUE;
	GLenum Rc = glewInit();
	if (Rc != GLEW_OK)
	{
		ShowError("render: failed to init GLEW - %s!", glewGetErrorString(Rc));
		return 1;
	}
	
	// set up models.
	for (usize i = 0; i < M_END__; ++i)
	{
		// generate GL state.
		glGenVertexArrays(1, &ModelData[i].Vao);
		glGenBuffers(1, &ModelData[i].Vbo);
		glGenBuffers(1, &ModelData[i].Ibo);
		
		glBindVertexArray(ModelData[i].Vao);
		glBindBuffer(GL_ARRAY_BUFFER, ModelData[i].Vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ModelData[i].Ibo);
		
		glBufferData(
			GL_ARRAY_BUFFER,
			8 * sizeof(f32) * ModelData[i].VertCnt,
			ModelData[i].VertData,
			GL_STATIC_DRAW
		);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(u32) * ModelData[i].IdxCnt,
			ModelData[i].IdxData,
			GL_STATIC_DRAW
		);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(5 * sizeof(f32)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}
	
	// set up shader programs.
	for (usize i = 0; i < SP_END__; ++i)
	{
		i32 Rc;
		static char Log[MAX_SHADER_LOG_LEN];

		// create vertex shader.
		u32 Vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(
			Vert,
			1,
			&ShaderProgramData[i].VertSrc,
			&ShaderProgramData[i].VertSrcLen
		);
		glCompileShader(Vert);
		glGetShaderiv(Vert, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Vert, MAX_SHADER_LOG_LEN, NULL, Log);
			ShowError("render: failed to compile vertex shader - %s!", Log);
			return 1;
		}
		
		// create fragment shader.
		u32 Frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(
			Frag,
			1,
			&ShaderProgramData[i].FragSrc,
			&ShaderProgramData[i].FragSrcLen
		);
		glCompileShader(Frag);
		glGetShaderiv(Frag, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Frag, MAX_SHADER_LOG_LEN, NULL, Log);
			ShowError("render: failed to compile fragment shader - %s!", Log);
			return 1;
		}
		
		// create program.
		u32 Prog = glCreateProgram();
		glAttachShader(Prog, Vert);
		glAttachShader(Prog, Frag);
		glLinkProgram(Prog);
		glGetProgramiv(Prog, GL_LINK_STATUS, &Rc);
		if (!Rc)
		{
			glGetProgramInfoLog(Prog, MAX_SHADER_LOG_LEN, NULL, Log);
			ShowError("render: failed to link shader program - %s!", Log);
			return 1;
		}
		
		ShaderProgramData[i].Vert = Vert;
		ShaderProgramData[i].Frag = Frag;
		ShaderProgramData[i].Prog = Prog;
	}
	
	// set up all textures.
	for (usize i = 0; i < T_END__; ++i)
	{
		// load texture.
		SDL_RWops *Rwops = SDL_RWFromConstMem(TextureData[i].Data, TextureData[i].Size);
		if (!Rwops)
		{
			ShowError("render: failed to create image texture RWops - %s!", SDL_GetError());
			return 1;
		}
		
		TextureData[i].Surf = IMG_Load_RW(Rwops, 1);
		if (!TextureData[i].Surf)
		{
			ShowError("render: failed to create SDL surface - %s!", SDL_GetError());
			return 1;
		}
		
		// generate GL state.
		glGenTextures(1, &TextureData[i].Tex);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureData[i].Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			TextureData[i].Surf->w,
			TextureData[i].Surf->h,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			TextureData[i].Surf->pixels
		);
	}
	
	// set GL state.
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	SetShaderProgram(SP_BASE);
	
	return 0;
}

void
SetupFrameRender(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// compute camera matrices.
	vec3 CamDst =
	{
		cos(g_Camera.Yaw) * cos(g_Camera.Pitch),
		sin(g_Camera.Pitch),
		sin(g_Camera.Yaw) * cos(g_Camera.Pitch)
	};
	glm_normalize(CamDst);
	glm_vec3_add(CamDst, g_Camera.Pos, CamDst);
	
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	f32 Aspect = (f32)w / (f32)h;
	
	mat4 ViewMat, ProjMat;
	glm_lookat(g_Camera.Pos, CamDst, CAM_UP_DIRECTION, ViewMat);
	glm_perspective(CAM_FOV, Aspect, CAM_CLIP_NEAR, CAM_CLIP_FAR, ProjMat);
	
	glUniformMatrix4fv(i_ViewMat, 1, GL_FALSE, (float *)ViewMat);
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (float *)ProjMat);
}

void
PresentFrame(void)
{
	SDL_GL_SwapWindow(Wnd);
}

void
OnWindowResize(i32 x, i32 y)
{
	glViewport(0, 0, x, y);
}

void
SetShaderProgram(enum ShaderProgram Prog)
{
	glUseProgram(ShaderProgramData[Prog].Prog);
	
	// set uniforms.
	i_ModelMat = glGetUniformLocation(ShaderProgramData[Prog].Prog, "i_ModelMat");
	i_ViewMat = glGetUniformLocation(ShaderProgramData[Prog].Prog, "i_ViewMat");
	i_ProjMat = glGetUniformLocation(ShaderProgramData[Prog].Prog, "i_ProjMat");
}

void
RenderModel(enum Model m, enum Texture t, vec3 Pos, vec3 Rot, vec3 Scale)
{
	// create transformation matrix.
	mat4 TransMat, RotMat, ScaleMat;
	glm_translate_make(TransMat, Pos);
	glm_euler(Rot, RotMat);
	glm_scale_make(ScaleMat, Scale);
	
	mat4 ModelMat = GLM_MAT4_IDENTITY_INIT;
	glm_mat4_mul(ModelMat, TransMat, ModelMat);
	glm_mat4_mul(ModelMat, RotMat, ModelMat);
	glm_mat4_mul(ModelMat, ScaleMat, ModelMat);
	
	// render model.
	glUniformMatrix4fv(i_ModelMat, 1, GL_FALSE, (float *)ModelMat);
	glBindTexture(GL_TEXTURE_2D, TextureData[t].Tex);
	glBindVertexArray(ModelData[m].Vao);
	glDrawElements(GL_TRIANGLES, ModelData[m].IdxCnt, GL_UNSIGNED_INT, 0);
}

static void
DeleteGlContext(void)
{
	SDL_GL_DeleteContext(GlContext);
}
