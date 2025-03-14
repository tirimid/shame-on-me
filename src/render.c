// raw render data.
#include "font/_5id_ttf.h"
#include "img/ceiling_png.h"
#include "img/dummy_png.h"
#include "img/floor_png.h"
#include "img/something_png.h"
#include "img/wall_png.h"
#include "model/cube_obj.h"
#include "model/plane_obj.h"
#include "shader/base_frag_glsl.h"
#include "shader/base_vert_glsl.h"
#include "shader/overlay_frag_glsl.h"
#include "shader/overlay_vert_glsl.h"
#include "shader/shadow_frag_glsl.h"
#include "shader/shadow_geo_glsl.h"
#include "shader/shadow_vert_glsl.h"

#define INCLUDE_MODEL(Name) \
	{ \
		.VertData = Name##_obj_VertData, \
		.IdxData = Name##_obj_IdxData, \
		.VertCnt = Name##_obj_VERT_CNT, \
		.IdxCnt = Name##_obj_IDX_CNT \
	}

#define INCLUDE_SHADER_PROGRAM(Name) \
	{ \
		.VertSrc = (char *)Name##_vert_glsl, \
		.FragSrc = (char *)Name##_frag_glsl, \
		.VertSrcLen = sizeof(Name##_vert_glsl), \
		.FragSrcLen = sizeof(Name##_frag_glsl) \
	}

#define INCLUDE_GEO_SHADER_PROGRAM(Name) \
	{ \
		.GeoSrc = (char *)Name##_geo_glsl, \
		.VertSrc = (char *)Name##_vert_glsl, \
		.FragSrc = (char *)Name##_frag_glsl, \
		.GeoSrcLen = sizeof(Name##_geo_glsl), \
		.VertSrcLen = sizeof(Name##_vert_glsl), \
		.FragSrcLen = sizeof(Name##_frag_glsl) \
	}

#define INCLUDE_TEXTURE(Name) \
	{ \
		.Data = Name##_png, \
		.Size = sizeof(Name##_png) \
	}

#define INCLUDE_FONT(Name) \
	{ \
		.Data = Name##_ttf, \
		.Size = sizeof(Name##_ttf) \
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
	char const *GeoSrc, *VertSrc, *FragSrc;
	i32 GeoSrcLen, VertSrcLen, FragSrcLen;
	u32 Geo, Vert, Frag;
	u32 Prog;
};

struct TextureData
{
	u8 const *Data;
	usize Size;
	SDL_Surface *Surf;
	u32 Tex;
};

struct FontData
{
	u8 const *Data;
	usize Size;
	TTF_Font *Font;
};

struct Camera g_Camera;

static void DeleteGlContext(void);

static SDL_Window *Wnd;
static SDL_GLContext GlContext;
static u32 i_ModelMat, i_ViewMat, i_ProjMat;
static u32 RFrameBuffer, RColorBuffer, RDepthBuffer;
static u32 i_Tex;
static vec4 Lights[CF_MAX_LIGHTS];
static u32 ShadowMaps[CF_MAX_LIGHTS];
static u32 ShadowFbos[CF_MAX_LIGHTS];
static usize LightCnt;
static u32 i_Lights, i_ShadowMaps;
static u32 i_LightPos, i_ShadowViewMats;

// data tables.
static struct ModelData ModelData[M_END__] =
{
	INCLUDE_MODEL(plane),
	INCLUDE_MODEL(cube)
};

static struct ShaderProgramData ShaderProgramData[SP_END__] =
{
	INCLUDE_SHADER_PROGRAM(base),
	INCLUDE_SHADER_PROGRAM(overlay),
	INCLUDE_GEO_SHADER_PROGRAM(shadow),
};

static struct TextureData TextureData[T_END__] =
{
	INCLUDE_TEXTURE(something),
	INCLUDE_TEXTURE(floor),
	INCLUDE_TEXTURE(ceiling),
	INCLUDE_TEXTURE(wall),
	INCLUDE_TEXTURE(dummy)
};

static struct FontData FontData[F_END__] =
{
	INCLUDE_FONT(_5id)
};

i32
R_Init(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	
	// set up rendering data and structures.
	Wnd = SDL_CreateWindow(
		CF_WND_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		0,
		0,
		CF_WND_FLAGS
	);
	if (!Wnd)
	{
		ShowError("render: failed to create window - %s!", SDL_GetError());
		return 1;
	}
	
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
	char ShaderLog[CF_MAX_LOG_LEN]; // TODO: rectify uninit.
	for (usize i = 0; i < SP_END__; ++i)
	{
		i32 Rc;
		
		// create geometry shader if present.
		u32 Geo = 0;
		if (ShaderProgramData[i].GeoSrc)
		{
			Geo = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(
				Geo,
				1,
				&ShaderProgramData[i].GeoSrc,
				&ShaderProgramData[i].GeoSrcLen
			);
			glCompileShader(Geo);
			glGetShaderiv(Geo, GL_COMPILE_STATUS, &Rc);
			if (!Rc)
			{
				glGetShaderInfoLog(Geo, CF_MAX_LOG_LEN, NULL, ShaderLog);
				ShowError("render: failed to compile geometry shader - %s!", ShaderLog);
				return 1;
			}
		}

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
			glGetShaderInfoLog(Vert, CF_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to compile vertex shader - %s!", ShaderLog);
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
			glGetShaderInfoLog(Frag, CF_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to compile fragment shader - %s!", ShaderLog);
			return 1;
		}
		
		// create program.
		u32 Prog = glCreateProgram();
		if (ShaderProgramData[i].GeoSrc)
			glAttachShader(Prog, Geo);
		glAttachShader(Prog, Vert);
		glAttachShader(Prog, Frag);
		glLinkProgram(Prog);
		glGetProgramiv(Prog, GL_LINK_STATUS, &Rc);
		if (!Rc)
		{
			glGetProgramInfoLog(Prog, CF_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to link shader program - %s!", ShaderLog);
			return 1;
		}
		
		ShaderProgramData[i].Geo = Geo;
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
			ShowError("render: failed to create SDL surface - %s!", IMG_GetError());
			return 1;
		}
		
		// generate GL state.
		glGenTextures(1, &TextureData[i].Tex);
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
	
	// set up all fonts.
	for (usize i = 0; i < F_END__; ++i)
	{
		SDL_RWops *Rwops = SDL_RWFromConstMem(FontData[i].Data, FontData[i].Size);
		if (!Rwops)
		{
			ShowError("render: failed to create font RWops - %s!", SDL_GetError());
			return 1;
		}
		
		FontData[i].Font = TTF_OpenFontRW(Rwops, 1, CF_FONT_PT);
		if (!FontData[i].Font)
		{
			ShowError("render: failed to open font - %s!", TTF_GetError());
			return 1;
		}
	}
	
	// create initial framebuffer data.
	glGenFramebuffers(1, &RFrameBuffer);
	glGenRenderbuffers(1, &RColorBuffer);
	glGenRenderbuffers(1, &RDepthBuffer);
	
	// create initial shadowmap data.
	glGenTextures(CF_MAX_LIGHTS, ShadowMaps);
	glGenFramebuffers(CF_MAX_LIGHTS, ShadowFbos);
	for (usize i = 0; i < CF_MAX_LIGHTS; ++i)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowMaps[i]);
		for (i32 j = 0; j < 6; ++j)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
				0,
				GL_DEPTH_COMPONENT,
				CF_SHADOW_MAP_SIZE,
				CF_SHADOW_MAP_SIZE,
				0,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				NULL
			);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		glBindFramebuffer(GL_FRAMEBUFFER, ShadowFbos[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowMaps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	
	// set GL state.
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	R_SetShaderProgram(SP_BASE);
	
	return 0;
}

usize
R_GetLightCnt(void)
{
	return LightCnt;
}

void
R_BeginShadow(usize Idx)
{
	vec3 LightPos = {Lights[Idx][0], Lights[Idx][1], Lights[Idx][2]};
	f32 Aspect = (f32)CF_SHADOW_MAP_SIZE / (f32)CF_SHADOW_MAP_SIZE;
	
	glViewport(0, 0, CF_SHADOW_MAP_SIZE, CF_SHADOW_MAP_SIZE);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ShadowFbos[Idx]);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	
	// compute matrices.
	mat4 ModelMat;
	glm_translate_make(ModelMat, LightPos);
	
	mat4 ShadowViewMats[6];
	glm_look(LightPos, (vec3){1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, ShadowViewMats[0]);
	glm_look(LightPos, (vec3){-1.0f, 0.0f, 0.0f}, (vec3){0.0f, -1.0f, 0.0f}, ShadowViewMats[1]);
	glm_look(LightPos, (vec3){0.0f, 1.0f, 0.0f}, (vec3){0.0f, 0.0f, 1.0f}, ShadowViewMats[2]);
	glm_look(LightPos, (vec3){0.0f, -1.0f, 0.0f}, (vec3){0.0f, 0.0f, -1.0f}, ShadowViewMats[3]);
	glm_look(LightPos, (vec3){0.0f, 0.0f, 1.0f}, (vec3){0.0f, -1.0f, 0.0f}, ShadowViewMats[4]);
	glm_look(LightPos, (vec3){0.0f, 0.0f, -1.0f}, (vec3){0.0f, -1.0f, 0.0f}, ShadowViewMats[5]);
	
	mat4 ProjMat;
	glm_perspective(GLM_PI / 2.0f, Aspect, CF_CAM_CLIP_NEAR, CF_CAM_CLIP_FAR, ProjMat);
	
	// upload uniforms.
	glUniform3fv(i_LightPos, 1, (f32 *)LightPos);
	glUniformMatrix4fv(i_ModelMat, 1, GL_FALSE, (f32 *)ModelMat);
	glUniformMatrix4fv(i_ShadowViewMats, 6, GL_FALSE, (f32 *)ShadowViewMats);
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
}

void
R_BeginFrame(void)
{
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	f32 Aspect = (f32)w / (f32)h;
	
	glViewport(0, 0, w / CF_PIXELATION, h / CF_PIXELATION);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	
	// compute matrices.
	vec3 CamDir =
	{
		cos(g_Camera.Yaw) * cos(g_Camera.Pitch),
		sin(g_Camera.Pitch),
		sin(g_Camera.Yaw) * cos(g_Camera.Pitch)
	};
	glm_normalize(CamDir);
	
	mat4 ViewMat, ProjMat;
	glm_look(g_Camera.Pos, CamDir, (vec3){0.0f, 1.0f, 0.0f}, ViewMat);
	glm_perspective(CF_CAM_FOV, Aspect, CF_CAM_CLIP_NEAR, CF_CAM_CLIP_FAR, ProjMat);
	
	// upload uniforms.
	glUniformMatrix4fv(i_ViewMat, 1, GL_FALSE, (f32 *)ViewMat);
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
	
	u32 ShadowMapSamplers[CF_MAX_LIGHTS] = {0};
	for (usize i = 0; i < CF_MAX_LIGHTS; ++i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowMaps[i]);
		ShadowMapSamplers[i] = 1 + i;
	}
	glUniform1iv(i_ShadowMaps, CF_MAX_LIGHTS, (i32 *)ShadowMapSamplers);
}

void
R_Present(void)
{
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, RFrameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0,
		0,
		w / CF_PIXELATION,
		h / CF_PIXELATION,
		0,
		0,
		w,
		h,
		GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);
	SDL_GL_SwapWindow(Wnd);
}

void
R_HandleResize(i32 x, i32 y)
{
	// regenerate frame buffer.
	glDeleteBuffers(1, &RColorBuffer);
	glGenRenderbuffers(1, &RColorBuffer);
	glDeleteBuffers(1, &RDepthBuffer);
	glGenRenderbuffers(1, &RDepthBuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, RFrameBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, RColorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, x / CF_PIXELATION, y / CF_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RColorBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, RDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, x / CF_PIXELATION, y / CF_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RDepthBuffer);
}

void
R_SetShaderProgram(enum ShaderProgram Prog)
{
	u32 p = ShaderProgramData[Prog].Prog;
	
	glUseProgram(p);
	
	// set uniforms.
	i_ModelMat = glGetUniformLocation(p, "i_ModelMat");
	i_ViewMat = glGetUniformLocation(p, "i_ViewMat");
	i_ProjMat = glGetUniformLocation(p, "i_ProjMat");
	i_Tex = glGetUniformLocation(p, "i_Tex");
	i_Lights = glGetUniformLocation(p, "i_Lights");
	i_ShadowMaps = glGetUniformLocation(p, "i_ShadowMaps");
	i_LightPos = glGetUniformLocation(p, "i_LightPos");
	i_ShadowViewMats = glGetUniformLocation(p, "i_ShadowViewMats");
}

void
R_Model(enum Model m, enum Texture t, vec3 Pos, vec3 Rot, vec3 Scale)
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
	glUniformMatrix4fv(i_ModelMat, 1, GL_FALSE, (f32 *)ModelMat);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureData[t].Tex);
	glUniform1i(i_Tex, 0);
	glBindVertexArray(ModelData[m].Vao);
	glDrawElements(GL_TRIANGLES, ModelData[m].IdxCnt, GL_UNSIGNED_INT, 0);
}

void
R_Light(vec3 Pos, f32 Intensity)
{
	if (LightCnt < CF_MAX_LIGHTS)
	{
		vec4 NewLight = {Pos[0], Pos[1], Pos[2], Intensity};
		glm_vec4_copy(NewLight, Lights[LightCnt++]);
		glUniform4fv(i_Lights, CF_MAX_LIGHTS, (f32 *)&Lights[0]);
	}
}

static void
DeleteGlContext(void)
{
	SDL_GL_DeleteContext(GlContext);
}
