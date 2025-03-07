#include "render.h"

#include <stddef.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_image.h>
#include <tinyobj_loader_c.h>

#include "util.h"

// raw render data.
#include "img/something_png.h"
#include "model/plane_mtl.h"
#include "model/plane_obj.h"
#include "shader/base_frag_glsl.h"
#include "shader/base_vert_glsl.h"

#define WND_TITLE "Fool Me Once, Shame on Me"
#define MAX_SHADER_LOG_LEN 512

#define INCLUDE_MODEL(Name) \
	{ \
		.ObjData = Name##_obj, \
		.MtlData = Name##_mtl, \
		.ObjDataLen = sizeof(Name##_obj), \
		.MtlDataLen = sizeof(Name##_mtl), \
		.Filename = #Name "_obj.h" \
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
	tinyobj_attrib_t Attrib;
	tinyobj_material_t *Materials;
	usize MaterialCnt;
	tinyobj_shape_t *Shapes;
	usize ShapeCnt;
	u8 const *ObjData;
	usize ObjDataLen;
	u8 const *MtlData;
	usize MtlDataLen;
	char *Filename;
	
	f32 *VertexData;
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

static void DeleteGlContext(void);
static void GetModelFileData(void *Ctx, char const *Filename, int const IsMtl, char const *ObjFilename, char **Data, size_t *Len);

static SDL_Window *Wnd;
static SDL_GLContext GlContext;

// data tables.
static struct ModelData ModelData[M_END__] =
{
	INCLUDE_MODEL(plane)
};

static struct ShaderProgramData ShaderProgramData[SP_END__] =
{
	INCLUDE_SHADER_PROGRAM(base_vert, base_frag)
};

static struct TextureData TextureData[T_END__] =
{
	INCLUDE_TEXTURE(something)
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
		// parse OBJ data.
		i32 Rc = tinyobj_parse_obj(
			&ModelData[i].Attrib,
			&ModelData[i].Shapes,
			&ModelData[i].ShapeCnt,
			&ModelData[i].Materials,
			&ModelData[i].MaterialCnt,
			ModelData[i].Filename,
			GetModelFileData,
			&i,
			TINYOBJ_FLAG_TRIANGULATE
		);
		if (Rc != TINYOBJ_SUCCESS)
		{
			ShowError("render: failed to load a model - ID %zu!", i);
			return 1;
		}
		
		// process OBJ data.
		ModelData[i].VertexData = malloc(5 * sizeof(f32) * ModelData[i].Attrib.num_vertices);
		for (usize j = 0; j < ModelData[i].Attrib.num_vertices; ++j)
		{
			ModelData[i].VertexData[5 * j] = ModelData[i].Attrib.vertices[3 * j];
			ModelData[i].VertexData[5 * j + 1] = ModelData[i].Attrib.vertices[3 * j + 1];
			ModelData[i].VertexData[5 * j + 2] = ModelData[i].Attrib.vertices[3 * j + 2];
			ModelData[i].VertexData[5 * j + 3] = ModelData[i].Attrib.texcoords[2 * j];
			ModelData[i].VertexData[5 * j + 4] = ModelData[i].Attrib.texcoords[2 * j + 1];
		}
		
		// generate GL state.
		glGenVertexArrays(1, &ModelData[i].Vao);
		glGenBuffers(1, &ModelData[i].Vbo);
		glGenBuffers(1, &ModelData[i].Ibo);
		
		glBindVertexArray(ModelData[i].Vao);
		glBindBuffer(GL_ARRAY_BUFFER, ModelData[i].Vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ModelData[i].Ibo);
		
		glBufferData(
			GL_ARRAY_BUFFER,
			5 * sizeof(float) * ModelData[i].Attrib.num_vertices,
			ModelData[i].VertexData,
			GL_STATIC_DRAW
		);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(int) * ModelData[i].Attrib.num_face_num_verts,
			ModelData[i].Attrib.face_num_verts,
			GL_STATIC_DRAW
		);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void *)(3 * sizeof(f32)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	return 0;
}

void
SetupFrameRender(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
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

static void
DeleteGlContext(void)
{
	SDL_GL_DeleteContext(GlContext);
}

static void
GetModelFileData(
	void *Ctx,
	char const *Filename,
	int const IsMtl,
	char const *ObjFilename,
	char **Data,
	size_t *Len
)
{
	enum Model m = *(usize *)Ctx;
	if (IsMtl)
	{
		*Data = (char *)ModelData[m].MtlData;
		*Len = ModelData[m].MtlDataLen;
	}
	else
	{
		*Data = (char *)ModelData[m].ObjData;
		*Len = ModelData[m].ObjDataLen;
	}
}
