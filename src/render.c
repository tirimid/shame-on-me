// raw render data.
#include "font/vcr_osd_mono_ttf.h"
#include "img/black_png.h"
#include "img/black50_png.h"
#include "img/c_clubs_6_png.h"
#include "img/c_clubs_7_png.h"
#include "img/c_clubs_8_png.h"
#include "img/c_clubs_9_png.h"
#include "img/c_clubs_10_png.h"
#include "img/c_clubs_j_png.h"
#include "img/c_clubs_q_png.h"
#include "img/c_clubs_k_png.h"
#include "img/c_clubs_a_png.h"
#include "img/c_diamonds_6_png.h"
#include "img/c_diamonds_7_png.h"
#include "img/c_diamonds_8_png.h"
#include "img/c_diamonds_9_png.h"
#include "img/c_diamonds_10_png.h"
#include "img/c_diamonds_j_png.h"
#include "img/c_diamonds_q_png.h"
#include "img/c_diamonds_k_png.h"
#include "img/c_diamonds_a_png.h"
#include "img/c_hearts_6_png.h"
#include "img/c_hearts_7_png.h"
#include "img/c_hearts_8_png.h"
#include "img/c_hearts_9_png.h"
#include "img/c_hearts_10_png.h"
#include "img/c_hearts_j_png.h"
#include "img/c_hearts_q_png.h"
#include "img/c_hearts_k_png.h"
#include "img/c_hearts_a_png.h"
#include "img/c_spades_6_png.h"
#include "img/c_spades_7_png.h"
#include "img/c_spades_8_png.h"
#include "img/c_spades_9_png.h"
#include "img/c_spades_10_png.h"
#include "img/c_spades_j_png.h"
#include "img/c_spades_q_png.h"
#include "img/c_spades_k_png.h"
#include "img/c_spades_a_png.h"
#include "img/ceiling_png.h"
#include "img/door_png.h"
#include "img/dummy_png.h"
#include "img/dummy_face_png.h"
#include "img/floor_png.h"
#include "img/glasses_dummy_png.h"
#include "img/glasses_dummy_face_png.h"
#include "img/something_png.h"
#include "img/wall_png.h"
#include "model/cube_obj.h"
#include "model/door_closed_obj.h"
#include "model/door_open_obj.h"
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
	char *GeoSrc, *VertSrc, *FragSrc;
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

static void PreprocShader(char *Src, usize Len);
static void DeleteGlContext(void);

static SDL_Window *Wnd;
static SDL_GLContext GlContext;
static u32 i_ModelMats, i_NormalMats, i_ViewMat, i_ProjMat;
static u32 RFrameBuffer, RColorBuffer, RDepthBuffer;
static u32 i_Tex;
static vec4 Lights[O_MAX_LIGHTS];
static u32 ShadowMaps[O_MAX_LIGHTS];
static u32 ShadowFbos[O_MAX_LIGHTS];
static usize LightCnt;
static u32 i_Lights, i_ShadowMaps;
static u32 i_LightPos, i_ShadowViewMats;
static f32 FadeBrightness = 0.0f;
static enum FadeStatus FadeStatus;
static u32 i_FadeBrightness;
static mat4 BatchModelMats[O_MAX_PLANE_BATCH];
static mat3 BatchNormalMats[O_MAX_PLANE_BATCH];
static usize BatchSize;

// data tables.
static struct ModelData ModelData[M_END__] =
{
	INCLUDE_MODEL(plane),
	INCLUDE_MODEL(cube),
	INCLUDE_MODEL(door_open),
	INCLUDE_MODEL(door_closed)
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
	INCLUDE_TEXTURE(dummy),
	INCLUDE_TEXTURE(black),
	INCLUDE_TEXTURE(dummy_face),
	INCLUDE_TEXTURE(black50),
	INCLUDE_TEXTURE(glasses_dummy),
	INCLUDE_TEXTURE(glasses_dummy_face),
	INCLUDE_TEXTURE(c_spades_6),
	INCLUDE_TEXTURE(c_spades_7),
	INCLUDE_TEXTURE(c_spades_8),
	INCLUDE_TEXTURE(c_spades_9),
	INCLUDE_TEXTURE(c_spades_10),
	INCLUDE_TEXTURE(c_spades_j),
	INCLUDE_TEXTURE(c_spades_q),
	INCLUDE_TEXTURE(c_spades_k),
	INCLUDE_TEXTURE(c_spades_a),
	INCLUDE_TEXTURE(c_diamonds_6),
	INCLUDE_TEXTURE(c_diamonds_7),
	INCLUDE_TEXTURE(c_diamonds_8),
	INCLUDE_TEXTURE(c_diamonds_9),
	INCLUDE_TEXTURE(c_diamonds_10),
	INCLUDE_TEXTURE(c_diamonds_j),
	INCLUDE_TEXTURE(c_diamonds_q),
	INCLUDE_TEXTURE(c_diamonds_k),
	INCLUDE_TEXTURE(c_diamonds_a),
	INCLUDE_TEXTURE(c_clubs_6),
	INCLUDE_TEXTURE(c_clubs_7),
	INCLUDE_TEXTURE(c_clubs_8),
	INCLUDE_TEXTURE(c_clubs_9),
	INCLUDE_TEXTURE(c_clubs_10),
	INCLUDE_TEXTURE(c_clubs_j),
	INCLUDE_TEXTURE(c_clubs_q),
	INCLUDE_TEXTURE(c_clubs_k),
	INCLUDE_TEXTURE(c_clubs_a),
	INCLUDE_TEXTURE(c_hearts_6),
	INCLUDE_TEXTURE(c_hearts_7),
	INCLUDE_TEXTURE(c_hearts_8),
	INCLUDE_TEXTURE(c_hearts_9),
	INCLUDE_TEXTURE(c_hearts_10),
	INCLUDE_TEXTURE(c_hearts_j),
	INCLUDE_TEXTURE(c_hearts_q),
	INCLUDE_TEXTURE(c_hearts_k),
	INCLUDE_TEXTURE(c_hearts_a),
	INCLUDE_TEXTURE(door)
};

static struct FontData FontData[F_END__] =
{
	INCLUDE_FONT(vcr_osd_mono)
};

i32
R_Init(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	
	// set up rendering data and structures.
	Wnd = SDL_CreateWindow(
		O_WND_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		O_WND_WIDTH,
		O_WND_HEIGHT,
		O_WND_FLAGS
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
	char ShaderLog[O_MAX_LOG_LEN];
	for (usize i = 0; i < SP_END__; ++i)
	{
		i32 Rc;
		
		// create geometry shader if present.
		u32 Geo = 0;
		if (ShaderProgramData[i].GeoSrc)
		{
			PreprocShader(ShaderProgramData[i].GeoSrc, ShaderProgramData[i].GeoSrcLen);
			Geo = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(
				Geo,
				1,
				(char const *const *)&ShaderProgramData[i].GeoSrc,
				&ShaderProgramData[i].GeoSrcLen
			);
			glCompileShader(Geo);
			glGetShaderiv(Geo, GL_COMPILE_STATUS, &Rc);
			if (!Rc)
			{
				glGetShaderInfoLog(Geo, O_MAX_LOG_LEN, NULL, ShaderLog);
				ShowError("render: failed to compile geometry shader - %s!", ShaderLog);
				return 1;
			}
		}

		// create vertex shader.
		PreprocShader(ShaderProgramData[i].VertSrc, ShaderProgramData[i].VertSrcLen);
		u32 Vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(
			Vert,
			1,
			(char const *const *)&ShaderProgramData[i].VertSrc,
			&ShaderProgramData[i].VertSrcLen
		);
		glCompileShader(Vert);
		glGetShaderiv(Vert, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Vert, O_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to compile vertex shader - %s!", ShaderLog);
			return 1;
		}
		
		// create fragment shader.
		PreprocShader(ShaderProgramData[i].FragSrc, ShaderProgramData[i].FragSrcLen);
		u32 Frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(
			Frag,
			1,
			(char const *const *)&ShaderProgramData[i].FragSrc,
			&ShaderProgramData[i].FragSrcLen
		);
		glCompileShader(Frag);
		glGetShaderiv(Frag, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Frag, O_MAX_LOG_LEN, NULL, ShaderLog);
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
			glGetProgramInfoLog(Prog, O_MAX_LOG_LEN, NULL, ShaderLog);
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
		
		FontData[i].Font = TTF_OpenFontRW(Rwops, 1, O_FONT_PT);
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
	glGenTextures(O_MAX_LIGHTS, ShadowMaps);
	glGenFramebuffers(O_MAX_LIGHTS, ShadowFbos);
	for (usize i = 0; i < O_MAX_LIGHTS; ++i)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowMaps[i]);
		for (i32 j = 0; j < 6; ++j)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + j,
				0,
				GL_DEPTH_COMPONENT,
				O_SHADOW_MAP_SIZE,
				O_SHADOW_MAP_SIZE,
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	R_SetShaderProgram(SP_BASE);
	
	return 0;
}

usize
R_GetLightCnt(void)
{
	return LightCnt;
}

void
R_GetRenderBounds(i32 *OutW, i32 *OutH)
{
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	
	*OutW = w / O_PIXELATION;
	*OutH = h / O_PIXELATION;
}

void
R_BeginShadow(usize Idx)
{
	vec3 LightPos = {Lights[Idx][0], Lights[Idx][1], Lights[Idx][2]};
	f32 Aspect = (f32)O_SHADOW_MAP_SIZE / (f32)O_SHADOW_MAP_SIZE;
	
	glViewport(0, 0, O_SHADOW_MAP_SIZE, O_SHADOW_MAP_SIZE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ShadowFbos[Idx]);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);
	
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
	glm_perspective(GLM_PI / 2.0f, Aspect, O_CAM_CLIP_NEAR, O_CAM_CLIP_FAR, ProjMat);
	
	// upload uniforms.
	glUniform3fv(i_LightPos, 1, (f32 *)LightPos);
	glUniformMatrix4fv(i_ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniformMatrix4fv(i_ShadowViewMats, 6, GL_FALSE, (f32 *)ShadowViewMats);
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
}

void
R_BeginBase(void)
{
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	f32 Aspect = (f32)w / (f32)h;
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RFrameBuffer);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
	glm_perspective(O_CAM_FOV, Aspect, O_CAM_CLIP_NEAR, O_CAM_CLIP_FAR, ProjMat);
	
	// upload uniforms.
	glUniformMatrix4fv(i_ViewMat, 1, GL_FALSE, (f32 *)ViewMat);
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
	
	glUniform1f(i_FadeBrightness, FadeBrightness);
	
	u32 ShadowMapSamplers[O_MAX_LIGHTS] = {0};
	for (usize i = 0; i < O_MAX_LIGHTS; ++i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowMaps[i]);
		ShadowMapSamplers[i] = 1 + i;
	}
	glUniform1iv(i_ShadowMaps, O_MAX_LIGHTS, (i32 *)ShadowMapSamplers);
}

void
R_BeginOverlay(void)
{
	i32 w, h;
	SDL_GetWindowSize(Wnd, &w, &h);
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RFrameBuffer);
	glDisable(GL_DEPTH_TEST);
	
	// compute matrices.
	mat4 ProjMat;
	glm_ortho(0.0f, w / O_PIXELATION, 0.0f, h / O_PIXELATION, -1.0f, 1.0f, ProjMat);
	
	// upload uniforms.
	glUniformMatrix4fv(i_ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
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
		w / O_PIXELATION,
		h / O_PIXELATION,
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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RColorBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, RDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RDepthBuffer);
}

void
R_SetShaderProgram(enum ShaderProgram Prog)
{
	u32 p = ShaderProgramData[Prog].Prog;
	
	glUseProgram(p);
	
	// set uniforms.
	i_ModelMats = glGetUniformLocation(p, "i_ModelMats");
	i_NormalMats = glGetUniformLocation(p, "i_NormalMats");
	i_ViewMat = glGetUniformLocation(p, "i_ViewMat");
	i_ProjMat = glGetUniformLocation(p, "i_ProjMat");
	i_Tex = glGetUniformLocation(p, "i_Tex");
	i_Lights = glGetUniformLocation(p, "i_Lights");
	i_ShadowMaps = glGetUniformLocation(p, "i_ShadowMaps");
	i_LightPos = glGetUniformLocation(p, "i_LightPos");
	i_ShadowViewMats = glGetUniformLocation(p, "i_ShadowViewMats");
	i_FadeBrightness = glGetUniformLocation(p, "i_FadeBrightness");
}

void
R_SetTexture(enum Texture t)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureData[t].Tex);
	glUniform1i(i_Tex, 0);
}

void
R_RenderModel(enum Model m, vec3 Pos, vec3 Rot, vec3 Scale)
{
	// create transformation matrices.
	mat4 ModelMat;
	MakeXformMat(Pos, Rot, Scale, ModelMat);
	
	mat3 NormalMat;
	MakeNormalMat(ModelMat, NormalMat);
	
	// render model.
	glUniformMatrix4fv(i_ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniformMatrix3fv(i_NormalMats, 1, GL_FALSE, (f32 *)NormalMat);
	glBindVertexArray(ModelData[m].Vao);
	glDrawElements(GL_TRIANGLES, ModelData[m].IdxCnt, GL_UNSIGNED_INT, 0);
}

i32
R_PutLight(vec3 Pos, f32 Intensity)
{
	if (LightCnt < O_MAX_LIGHTS)
	{
		vec4 NewLight = {Pos[0], Pos[1], Pos[2], Intensity};
		glm_vec4_copy(NewLight, Lights[LightCnt]);
		glUniform4fv(i_Lights, O_MAX_LIGHTS, (f32 *)&Lights[0]);
		return LightCnt++;
	}
	
	return -1;
}

void
R_RenderRect(enum Texture t, i32 x, i32 y, i32 w, i32 h)
{
	vec3 Pos = {x + w / 2.0f, y + h / 2.0f, 0.0f};
	vec3 Rot = {GLM_PI / 2.0f, GLM_PI, GLM_PI};
	vec3 Scale = {w / 2.0f, 1.0f, h / 2.0f};
	
	R_SetTexture(t);
	R_RenderModel(M_PLANE, Pos, Rot, Scale);
}

void
R_RenderText(enum Font f, char const *Text, i32 x, i32 y, i32 w, i32 h)
{
	// TTF render out text to create texture.
	SDL_Surface *SolidSurf = TTF_RenderUTF8_Solid_Wrapped(
		FontData[f].Font,
		Text,
		(SDL_Color)O_FONT_COLOR,
		w
	);
	SolidSurf->h = SolidSurf->h < h ? SolidSurf->h : h;
	
	SDL_Surface *RgbaSurf = SDL_ConvertSurfaceFormat(SolidSurf, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_FreeSurface(SolidSurf);
	
	u32 Tex;
	glGenTextures(1, &Tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		RgbaSurf->w,
		RgbaSurf->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		RgbaSurf->pixels
	);
	
	y += h - RgbaSurf->h;
	
	// create transformation matrix.
	vec3 Pos =
	{
		x + RgbaSurf->w / 2.0f,
		y + RgbaSurf->h / 2.0f,
		0.0f
	};
	
	vec3 Scale =
	{
		RgbaSurf->w / 2.0f,
		1.0f,
		RgbaSurf->h / 2.0f
	};
	
	mat4 TransMat, RotMat, ScaleMat;
	glm_translate_make(TransMat, Pos);
	glm_euler((vec3){GLM_PI / 2.0f, GLM_PI, GLM_PI}, RotMat);
	glm_scale_make(ScaleMat, Scale);
	
	mat4 ModelMat = GLM_MAT4_IDENTITY_INIT;
	glm_mat4_mul(ModelMat, TransMat, ModelMat);
	glm_mat4_mul(ModelMat, RotMat, ModelMat);
	glm_mat4_mul(ModelMat, ScaleMat, ModelMat);
	
	// render model.
	glUniformMatrix4fv(i_ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniform1i(i_Tex, GL_TEXTURE0);
	glBindVertexArray(ModelData[M_PLANE].Vao);
	glDrawElements(GL_TRIANGLES, ModelData[M_PLANE].IdxCnt, GL_UNSIGNED_INT, NULL);
	
	glDeleteTextures(1, &Tex);
	SDL_FreeSurface(RgbaSurf);
}

void
R_BatchRenderPlane(vec3 Pos, vec3 Rot, vec3 Scale)
{
	if (BatchSize >= O_MAX_PLANE_BATCH)
		R_FlushPlaneBatch();
	
	MakeXformMat(Pos, Rot, Scale, BatchModelMats[BatchSize]);
	MakeNormalMat(BatchModelMats[BatchSize], BatchNormalMats[BatchSize]);
	++BatchSize;
}

void
R_FlushPlaneBatch(void)
{
	if (BatchSize == 0)
		return;
	
	glUniformMatrix4fv(i_ModelMats, BatchSize, GL_FALSE, (f32 *)BatchModelMats);
	glUniformMatrix3fv(i_NormalMats, BatchSize, GL_FALSE, (f32 *)BatchNormalMats);
	glBindVertexArray(ModelData[M_PLANE].Vao);
	glDrawElementsInstanced(GL_TRIANGLES, ModelData[M_PLANE].IdxCnt, GL_UNSIGNED_INT, NULL, BatchSize);
	BatchSize = 0;
}

void
R_Update(void)
{
	// update fade brightness.
	if (FadeStatus)
	{
		FadeBrightness += O_FADE_SPEED;
		FadeBrightness = FadeBrightness > 1.0f ? 1.0f : FadeBrightness;
	}
	else
	{
		FadeBrightness -= O_FADE_SPEED;
		FadeBrightness = FadeBrightness < 0.0f ? 0.0f : FadeBrightness;
	}
}

void
R_Fade(enum FadeStatus Fs)
{
	FadeStatus = Fs;
}

static void
PreprocShader(char *Src, usize Len)
{
	// substitute all constants for in-game values prior to shader compilation.
	for (usize i = 0; i < Len; ++i)
	{
		if (Len - i >= 13 && !strncmp(&Src[i], "$O_MAX_LIGHTS", 13))
		{
			usize Cl = snprintf(&Src[i], 13, "%u", O_MAX_LIGHTS);
			memset(&Src[i + Cl], ' ', 13 - Cl);
		}
		else if (Len - i >= 16 && !strncmp(&Src[i], "$O_AMBIENT_LIGHT", 16))
		{
			usize Cl = snprintf(&Src[i], 16, "%f", O_AMBIENT_LIGHT);
			memset(&Src[i + Cl], ' ', 16 - Cl);
		}
		else if (Len - i >= 13 && !strncmp(&Src[i], "$O_LIGHT_STEP", 13))
		{
			usize Cl = snprintf(&Src[i], 13, "%f", O_LIGHT_STEP);
			memset(&Src[i + Cl], ' ', 13 - Cl);
		}
		else if (Len - i >= 14 && !strncmp(&Src[i], "$O_SHADOW_BIAS", 14))
		{
			usize Cl = snprintf(&Src[i], 14, "%f", O_SHADOW_BIAS);
			memset(&Src[i + Cl], ' ', 14 - Cl);
		}
		else if (Len - i >= 15 && !strncmp(&Src[i], "$O_CAM_CLIP_FAR", 15))
		{
			usize Cl = snprintf(&Src[i], 15, "%f", O_CAM_CLIP_FAR);
			memset(&Src[i + Cl], ' ', 15 - Cl);
		}
		else if (Len - i >= 18 && !strncmp(&Src[i], "$O_MAX_PLANE_BATCH", 18))
		{
			usize Cl = snprintf(&Src[i], 18, "%u", O_MAX_PLANE_BATCH);
			memset(&Src[i + Cl], ' ', 18 - Cl);
		}
	}
}

static void
DeleteGlContext(void)
{
	SDL_GL_DeleteContext(GlContext);
}
