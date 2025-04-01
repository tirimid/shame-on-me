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
#include "img/eyes_dummy_png.h"
#include "img/eyes_dummy_face_png.h"
#include "img/floor_png.h"
#include "img/glasses_dummy_png.h"
#include "img/glasses_dummy_face_png.h"
#include "img/lightbulb_png.h"
#include "img/something_png.h"
#include "img/table_png.h"
#include "img/wall_png.h"
#include "img/window_png.h"
#include "model/cube_obj.h"
#include "model/door_closed_obj.h"
#include "model/door_open_obj.h"
#include "model/lightbulb_obj.h"
#include "model/plane_obj.h"
#include "model/table_obj.h"
#include "model/window_obj.h"
#include "shader/base_frag_glsl.h"
#include "shader/base_vert_glsl.h"
#include "shader/overlay_frag_glsl.h"
#include "shader/overlay_vert_glsl.h"
#include "shader/shadow_frag_glsl.h"
#include "shader/shadow_geo_glsl.h"
#include "shader/shadow_vert_glsl.h"

#define R_INCLUDE_MODEL(Name) \
	{ \
		.VertData = Name##_obj_VertData, \
		.IdxData = Name##_obj_IdxData, \
		.VertCnt = Name##_obj_VERT_CNT, \
		.IdxCnt = Name##_obj_IDX_CNT \
	}

#define R_INCLUDE_SHADER(Name) \
	{ \
		.VertSrc = (char *)Name##_vert_glsl, \
		.FragSrc = (char *)Name##_frag_glsl, \
		.VertSrcLen = sizeof(Name##_vert_glsl), \
		.FragSrcLen = sizeof(Name##_frag_glsl) \
	}

#define R_INCLUDE_GEO_SHADER(Name) \
	{ \
		.GeoSrc = (char *)Name##_geo_glsl, \
		.VertSrc = (char *)Name##_vert_glsl, \
		.FragSrc = (char *)Name##_frag_glsl, \
		.GeoSrcLen = sizeof(Name##_geo_glsl), \
		.VertSrcLen = sizeof(Name##_vert_glsl), \
		.FragSrcLen = sizeof(Name##_frag_glsl) \
	}

#define R_INCLUDE_TEXTURE(Name) \
	{ \
		.Data = Name##_png, \
		.Size = sizeof(Name##_png) \
	}

#define R_INCLUDE_FONT(Name) \
	{ \
		.Data = Name##_ttf, \
		.Size = sizeof(Name##_ttf) \
	}

typedef struct R_ModelData
{
	f32 const *VertData;
	usize VertCnt;
	u32 const *IdxData;
	usize IdxCnt;
	u32 VBO, IBO, VAO;
} R_ModelData;

typedef struct R_ShaderData
{
	char *GeoSrc, *VertSrc, *FragSrc;
	i32 GeoSrcLen, VertSrcLen, FragSrcLen;
	u32 Prog;
} R_ShaderData;

typedef struct R_TextureData
{
	u8 const *Data;
	usize Size;
	SDL_Surface *Surf;
	u32 Tex;
} R_TextureData;

typedef struct R_FontData
{
	u8 const *Data;
	usize Size;
	TTF_Font *Font;
} R_FontData;

typedef struct R_UniformData
{
	u32 ModelMats, NormalMats;
	u32 ViewMat, ProjMat;
	u32 Tex;
	u32 Lights, ShadowMaps;
	u32 LightPos, ShadowViewMats;
	u32 FadeBrightness;
} R_UniformData;

typedef struct R_RenderState
{
	u32 FrameBuffer, ColorBuffer, DepthBuffer;
	vec4 Lights[O_MAX_LIGHTS];
	u32 ShadowMaps[O_MAX_LIGHTS];
	u32 ShadowFBOs[O_MAX_LIGHTS];
	usize LightCnt;
	f32 FadeBrightness;
	R_FadeStatus FadeStatus;
	mat4 BatchModelMats[O_MAX_TILE_BATCH];
	mat3 BatchNormalMats[O_MAX_TILE_BATCH];
	usize BatchSize;
} R_RenderState;

R_Camera R_Cam;

static void R_PreprocShader(char *Src, usize Len);
static void R_DeleteGlContext(void);

static SDL_Window *R_Wnd;
static SDL_GLContext R_GlContext;
static R_RenderState R_State;
static R_UniformData R_Uniforms;

// data tables.
static R_ModelData R_Models[R_M_END__] =
{
	R_INCLUDE_MODEL(plane),
	R_INCLUDE_MODEL(cube),
	R_INCLUDE_MODEL(door_open),
	R_INCLUDE_MODEL(door_closed),
	R_INCLUDE_MODEL(table),
	R_INCLUDE_MODEL(window),
	R_INCLUDE_MODEL(lightbulb)
};

static R_ShaderData R_Shaders[R_S_END__] =
{
	R_INCLUDE_SHADER(base),
	R_INCLUDE_SHADER(overlay),
	R_INCLUDE_GEO_SHADER(shadow)
};

static R_TextureData R_Textures[R_T_END__] =
{
	R_INCLUDE_TEXTURE(something),
	R_INCLUDE_TEXTURE(floor),
	R_INCLUDE_TEXTURE(ceiling),
	R_INCLUDE_TEXTURE(wall),
	R_INCLUDE_TEXTURE(dummy),
	R_INCLUDE_TEXTURE(black),
	R_INCLUDE_TEXTURE(dummy_face),
	R_INCLUDE_TEXTURE(black50),
	R_INCLUDE_TEXTURE(glasses_dummy),
	R_INCLUDE_TEXTURE(glasses_dummy_face),
	R_INCLUDE_TEXTURE(c_spades_6),
	R_INCLUDE_TEXTURE(c_spades_7),
	R_INCLUDE_TEXTURE(c_spades_8),
	R_INCLUDE_TEXTURE(c_spades_9),
	R_INCLUDE_TEXTURE(c_spades_10),
	R_INCLUDE_TEXTURE(c_spades_j),
	R_INCLUDE_TEXTURE(c_spades_q),
	R_INCLUDE_TEXTURE(c_spades_k),
	R_INCLUDE_TEXTURE(c_spades_a),
	R_INCLUDE_TEXTURE(c_diamonds_6),
	R_INCLUDE_TEXTURE(c_diamonds_7),
	R_INCLUDE_TEXTURE(c_diamonds_8),
	R_INCLUDE_TEXTURE(c_diamonds_9),
	R_INCLUDE_TEXTURE(c_diamonds_10),
	R_INCLUDE_TEXTURE(c_diamonds_j),
	R_INCLUDE_TEXTURE(c_diamonds_q),
	R_INCLUDE_TEXTURE(c_diamonds_k),
	R_INCLUDE_TEXTURE(c_diamonds_a),
	R_INCLUDE_TEXTURE(c_clubs_6),
	R_INCLUDE_TEXTURE(c_clubs_7),
	R_INCLUDE_TEXTURE(c_clubs_8),
	R_INCLUDE_TEXTURE(c_clubs_9),
	R_INCLUDE_TEXTURE(c_clubs_10),
	R_INCLUDE_TEXTURE(c_clubs_j),
	R_INCLUDE_TEXTURE(c_clubs_q),
	R_INCLUDE_TEXTURE(c_clubs_k),
	R_INCLUDE_TEXTURE(c_clubs_a),
	R_INCLUDE_TEXTURE(c_hearts_6),
	R_INCLUDE_TEXTURE(c_hearts_7),
	R_INCLUDE_TEXTURE(c_hearts_8),
	R_INCLUDE_TEXTURE(c_hearts_9),
	R_INCLUDE_TEXTURE(c_hearts_10),
	R_INCLUDE_TEXTURE(c_hearts_j),
	R_INCLUDE_TEXTURE(c_hearts_q),
	R_INCLUDE_TEXTURE(c_hearts_k),
	R_INCLUDE_TEXTURE(c_hearts_a),
	R_INCLUDE_TEXTURE(door),
	R_INCLUDE_TEXTURE(table),
	R_INCLUDE_TEXTURE(window),
	R_INCLUDE_TEXTURE(lightbulb),
	R_INCLUDE_TEXTURE(eyes_dummy),
	R_INCLUDE_TEXTURE(eyes_dummy_face)
};

static R_FontData R_Fonts[R_F_END__] =
{
	R_INCLUDE_FONT(vcr_osd_mono)
};

i32
R_Init(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
	
	// set up rendering data and structures.
	R_Wnd = SDL_CreateWindow(
		O_WND_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		O_WND_WIDTH,
		O_WND_HEIGHT,
		O_WND_FLAGS
	);
	if (!R_Wnd)
	{
		ShowError("render: failed to create window - %s!", SDL_GetError());
		return 1;
	}
	
	R_GlContext = SDL_GL_CreateContext(R_Wnd);
	if (!R_GlContext)
	{
		ShowError("render: failed to create GL context - %s!", SDL_GetError());
		return 1;
	}
	atexit(R_DeleteGlContext);
	
	glewExperimental = GL_TRUE;
	GLenum Rc = glewInit();
	if (Rc != GLEW_OK)
	{
		ShowError("render: failed to init GLEW - %s!", glewGetErrorString(Rc));
		return 1;
	}
	
	// set up models.
	for (usize i = 0; i < R_M_END__; ++i)
	{
		// generate GL state.
		glGenVertexArrays(1, &R_Models[i].VAO);
		glGenBuffers(1, &R_Models[i].VBO);
		glGenBuffers(1, &R_Models[i].IBO);
		
		glBindVertexArray(R_Models[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, R_Models[i].VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, R_Models[i].IBO);
		
		glBufferData(
			GL_ARRAY_BUFFER,
			8 * sizeof(f32) * R_Models[i].VertCnt,
			R_Models[i].VertData,
			GL_STATIC_DRAW
		);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(u32) * R_Models[i].IdxCnt,
			R_Models[i].IdxData,
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
	for (usize i = 0; i < R_S_END__; ++i)
	{
		i32 Rc;
		
		// create geometry shader if present.
		u32 Geo = 0;
		if (R_Shaders[i].GeoSrc)
		{
			R_PreprocShader(R_Shaders[i].GeoSrc, R_Shaders[i].GeoSrcLen);
			Geo = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(
				Geo,
				1,
				(char const *const *)&R_Shaders[i].GeoSrc,
				&R_Shaders[i].GeoSrcLen
			);
			glCompileShader(Geo);
			glGetShaderiv(Geo, GL_COMPILE_STATUS, &Rc);
			if (!Rc)
			{
				glGetShaderInfoLog(Geo, O_MAX_LOG_LEN, NULL, ShaderLog);
				ShowError("render: failed to compile geometry shader %zu - %s!", i, ShaderLog);
				return 1;
			}
		}

		// create vertex shader.
		R_PreprocShader(R_Shaders[i].VertSrc, R_Shaders[i].VertSrcLen);
		u32 Vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(
			Vert,
			1,
			(char const *const *)&R_Shaders[i].VertSrc,
			&R_Shaders[i].VertSrcLen
		);
		glCompileShader(Vert);
		glGetShaderiv(Vert, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Vert, O_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to compile vertex shader %zu - %s!", i, ShaderLog);
			return 1;
		}
		
		// create fragment shader.
		R_PreprocShader(R_Shaders[i].FragSrc, R_Shaders[i].FragSrcLen);
		u32 Frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(
			Frag,
			1,
			(char const *const *)&R_Shaders[i].FragSrc,
			&R_Shaders[i].FragSrcLen
		);
		glCompileShader(Frag);
		glGetShaderiv(Frag, GL_COMPILE_STATUS, &Rc);
		if (!Rc)
		{
			glGetShaderInfoLog(Frag, O_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to compile fragment shader %zu - %s!", i, ShaderLog);
			return 1;
		}
		
		// create program.
		u32 Prog = glCreateProgram();
		if (R_Shaders[i].GeoSrc) {glAttachShader(Prog, Geo);}
		glAttachShader(Prog, Vert);
		glAttachShader(Prog, Frag);
		glLinkProgram(Prog);
		glGetProgramiv(Prog, GL_LINK_STATUS, &Rc);
		if (!Rc)
		{
			glGetProgramInfoLog(Prog, O_MAX_LOG_LEN, NULL, ShaderLog);
			ShowError("render: failed to link shader program %zu - %s!", i, ShaderLog);
			return 1;
		}
		
		if (Geo) {glDeleteShader(Geo);}
		glDeleteShader(Vert);
		glDeleteShader(Frag);
		
		R_Shaders[i].Prog = Prog;
	}
	
	// set up all textures.
	for (usize i = 0; i < R_T_END__; ++i)
	{
		// load texture.
		SDL_RWops *RWOps = SDL_RWFromConstMem(R_Textures[i].Data, R_Textures[i].Size);
		if (!RWOps)
		{
			ShowError("render: failed to create image texture RWOps - %s!", SDL_GetError());
			return 1;
		}
		
		R_Textures[i].Surf = IMG_Load_RW(RWOps, 1);
		if (!R_Textures[i].Surf)
		{
			ShowError("render: failed to create SDL surface - %s!", IMG_GetError());
			return 1;
		}
		
		// generate GL state.
		glGenTextures(1, &R_Textures[i].Tex);
		glBindTexture(GL_TEXTURE_2D, R_Textures[i].Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			R_Textures[i].Surf->w,
			R_Textures[i].Surf->h,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			R_Textures[i].Surf->pixels
		);
	}
	
	// set up all fonts.
	for (usize i = 0; i < R_F_END__; ++i)
	{
		SDL_RWops *RWOps = SDL_RWFromConstMem(R_Fonts[i].Data, R_Fonts[i].Size);
		if (!RWOps)
		{
			ShowError("render: failed to create font RWOps - %s!", SDL_GetError());
			return 1;
		}
		
		R_Fonts[i].Font = TTF_OpenFontRW(RWOps, 1, O_FONT_PT);
		if (!R_Fonts[i].Font)
		{
			ShowError("render: failed to open font - %s!", TTF_GetError());
			return 1;
		}
	}
	
	// create initial framebuffer data.
	glGenFramebuffers(1, &R_State.FrameBuffer);
	glGenRenderbuffers(1, &R_State.ColorBuffer);
	glGenRenderbuffers(1, &R_State.DepthBuffer);
	
	// create initial shadowmap data.
	glGenTextures(O_MAX_LIGHTS, R_State.ShadowMaps);
	glGenFramebuffers(O_MAX_LIGHTS, R_State.ShadowFBOs);
	for (usize i = 0; i < O_MAX_LIGHTS; ++i)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, R_State.ShadowMaps[i]);
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
		
		glBindFramebuffer(GL_FRAMEBUFFER, R_State.ShadowFBOs[i]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, R_State.ShadowMaps[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	
	// set GL state.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//R_SetShader(R_S_BASE);
	
	return 0;
}

bool
R_LightEnabled(usize Idx)
{
	return R_State.Lights[Idx][3] > 0.0f;
}

void
R_GetRenderBounds(i32 *OutW, i32 *OutH)
{
	i32 w, h;
	SDL_GetWindowSize(R_Wnd, &w, &h);
	
	*OutW = w / O_PIXELATION;
	*OutH = h / O_PIXELATION;
}

void
R_BeginShadow(usize Idx)
{
	vec3 LightPos = {R_State.Lights[Idx][0], R_State.Lights[Idx][1], R_State.Lights[Idx][2]};
	f32 Aspect = (f32)O_SHADOW_MAP_SIZE / (f32)O_SHADOW_MAP_SIZE;
	
	glViewport(0, 0, O_SHADOW_MAP_SIZE, O_SHADOW_MAP_SIZE);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, R_State.ShadowFBOs[Idx]);
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
	glUniform3fv(R_Uniforms.LightPos, 1, (f32 *)LightPos);
	glUniformMatrix4fv(R_Uniforms.ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniformMatrix4fv(R_Uniforms.ShadowViewMats, 6, GL_FALSE, (f32 *)ShadowViewMats);
	glUniformMatrix4fv(R_Uniforms.ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
	glUniform4fv(R_Uniforms.Lights, O_MAX_LIGHTS, (f32 *)&R_State.Lights[0]);
}

void
R_BeginBase(void)
{
	i32 w, h;
	SDL_GetWindowSize(R_Wnd, &w, &h);
	f32 Aspect = (f32)w / (f32)h;
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, R_State.FrameBuffer);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// compute matrices.
	vec3 CamDir =
	{
		cos(R_Cam.Yaw) * cos(R_Cam.Pitch),
		sin(R_Cam.Pitch),
		sin(R_Cam.Yaw) * cos(R_Cam.Pitch)
	};
	glm_normalize(CamDir);
	
	mat4 ViewMat, ProjMat;
	glm_look(R_Cam.Pos, CamDir, (vec3){0.0f, 1.0f, 0.0f}, ViewMat);
	glm_perspective(O_CAM_FOV, Aspect, O_CAM_CLIP_NEAR, O_CAM_CLIP_FAR, ProjMat);
	
	// upload uniforms.
	glUniformMatrix4fv(R_Uniforms.ViewMat, 1, GL_FALSE, (f32 *)ViewMat);
	glUniformMatrix4fv(R_Uniforms.ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
	glUniform4fv(R_Uniforms.Lights, O_MAX_LIGHTS, (f32 *)&R_State.Lights[0]);
	glUniform1f(R_Uniforms.FadeBrightness, R_State.FadeBrightness);
	
	u32 ShadowMapSamplers[O_MAX_LIGHTS] = {0};
	for (usize i = 0; i < O_MAX_LIGHTS; ++i)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, R_State.ShadowMaps[i]);
		ShadowMapSamplers[i] = 1 + i;
	}
	glUniform1iv(R_Uniforms.ShadowMaps, O_MAX_LIGHTS, (i32 *)ShadowMapSamplers);
}

void
R_BeginOverlay(void)
{
	i32 w, h;
	SDL_GetWindowSize(R_Wnd, &w, &h);
	
	glViewport(0, 0, w / O_PIXELATION, h / O_PIXELATION);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, R_State.FrameBuffer);
	glDisable(GL_DEPTH_TEST);
	
	// compute matrices.
	mat4 ProjMat;
	glm_ortho(0.0f, w / O_PIXELATION, 0.0f, h / O_PIXELATION, -1.0f, 1.0f, ProjMat);
	
	// upload uniforms.
	glUniformMatrix4fv(R_Uniforms.ProjMat, 1, GL_FALSE, (f32 *)ProjMat);
}

void
R_Present(void)
{
	i32 w, h;
	SDL_GetWindowSize(R_Wnd, &w, &h);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, R_State.FrameBuffer);
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
	SDL_GL_SwapWindow(R_Wnd);
}

void
R_HandleResize(i32 x, i32 y)
{
	// regenerate frame buffer.
	glDeleteBuffers(1, &R_State.ColorBuffer);
	glGenRenderbuffers(1, &R_State.ColorBuffer);
	glDeleteBuffers(1, &R_State.DepthBuffer);
	glGenRenderbuffers(1, &R_State.DepthBuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, R_State.FrameBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, R_State.ColorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, R_State.ColorBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, R_State.DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, x / O_PIXELATION, y / O_PIXELATION);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, R_State.DepthBuffer);
}

void
R_SetShader(R_Shader s)
{
	u32 p = R_Shaders[s].Prog;
	
	glUseProgram(p);
	
	// set uniforms.
	R_Uniforms.ModelMats = glGetUniformLocation(p, "i_ModelMats");
	R_Uniforms.NormalMats = glGetUniformLocation(p, "i_NormalMats");
	R_Uniforms.ViewMat = glGetUniformLocation(p, "i_ViewMat");
	R_Uniforms.ProjMat = glGetUniformLocation(p, "i_ProjMat");
	R_Uniforms.Tex = glGetUniformLocation(p, "i_Tex");
	R_Uniforms.Lights = glGetUniformLocation(p, "i_Lights");
	R_Uniforms.ShadowMaps = glGetUniformLocation(p, "i_ShadowMaps");
	R_Uniforms.LightPos = glGetUniformLocation(p, "i_LightPos");
	R_Uniforms.ShadowViewMats = glGetUniformLocation(p, "i_ShadowViewMats");
	R_Uniforms.FadeBrightness = glGetUniformLocation(p, "i_FadeBrightness");
}

void
R_SetTexture(R_Texture t)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, R_Textures[t].Tex);
	glUniform1i(R_Uniforms.Tex, 0);
}

void
R_RenderModel(R_Model m, vec3 Pos, vec3 Rot, vec3 Scale)
{
	// create transformation matrices.
	mat4 ModelMat;
	MakeXformMat(Pos, Rot, Scale, ModelMat);
	
	mat3 NormalMat;
	MakeNormalMat(ModelMat, NormalMat);
	
	// render model.
	glUniformMatrix4fv(R_Uniforms.ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniformMatrix3fv(R_Uniforms.NormalMats, 1, GL_FALSE, (f32 *)NormalMat);
	glBindVertexArray(R_Models[m].VAO);
	glDrawElements(GL_TRIANGLES, R_Models[m].IdxCnt, GL_UNSIGNED_INT, 0);
}

i32
R_PutLight(vec3 Pos, f32 Intensity)
{
	if (R_State.LightCnt >= O_MAX_LIGHTS) {return -1;}
	
	vec4 NewLight = {Pos[0], Pos[1], Pos[2], Intensity};
	glm_vec4_copy(NewLight, R_State.Lights[R_State.LightCnt]);
	return R_State.LightCnt++;
}

void
R_SetLightIntensity(usize Idx, f32 Intensity)
{
	R_State.Lights[Idx][3] = Intensity;
}

void
R_RenderRect(R_Texture t, i32 x, i32 y, i32 w, i32 h)
{
	vec3 Pos = {x + w / 2.0f, y + h / 2.0f, 0.0f};
	vec3 Rot = {GLM_PI / 2.0f, GLM_PI, GLM_PI};
	vec3 Scale = {w / 2.0f, 1.0f, h / 2.0f};
	
	R_SetTexture(t);
	R_RenderModel(R_M_PLANE, Pos, Rot, Scale);
}

void
R_RenderText(R_Font f, char const *Text, i32 x, i32 y, i32 w, i32 h)
{
	// TTF render out text to create texture.
	SDL_Surface *SolidSurf = TTF_RenderUTF8_Solid_Wrapped(
		R_Fonts[f].Font,
		Text,
		(SDL_Color)O_FONT_COLOR,
		w
	);
	SolidSurf->h = SolidSurf->h < h ? SolidSurf->h : h;
	
	SDL_Surface *RGBASurf = SDL_ConvertSurfaceFormat(SolidSurf, SDL_PIXELFORMAT_RGBA8888, 0);
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
		RGBASurf->w,
		RGBASurf->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		RGBASurf->pixels
	);
	
	y += h - RGBASurf->h;
	
	// create transformation matrix.
	vec3 Pos =
	{
		x + RGBASurf->w / 2.0f,
		y + RGBASurf->h / 2.0f,
		0.0f
	};
	
	vec3 Scale =
	{
		RGBASurf->w / 2.0f,
		1.0f,
		RGBASurf->h / 2.0f
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
	glUniformMatrix4fv(R_Uniforms.ModelMats, 1, GL_FALSE, (f32 *)ModelMat);
	glUniform1i(R_Uniforms.Tex, GL_TEXTURE0);
	glBindVertexArray(R_Models[R_M_PLANE].VAO);
	glDrawElements(GL_TRIANGLES, R_Models[R_M_PLANE].IdxCnt, GL_UNSIGNED_INT, NULL);
	
	glDeleteTextures(1, &Tex);
	SDL_FreeSurface(RGBASurf);
}

void
R_BatchRenderTile(vec3 Pos, vec3 Rot, vec3 Scale)
{
	if (R_State.BatchSize >= O_MAX_TILE_BATCH) {R_FlushTileBatch();}
	
	MakeXformMat(Pos, Rot, Scale, R_State.BatchModelMats[R_State.BatchSize]);
	MakeNormalMat(R_State.BatchModelMats[R_State.BatchSize], R_State.BatchNormalMats[R_State.BatchSize]);
	++R_State.BatchSize;
}

void
R_FlushTileBatch(void)
{
	if (R_State.BatchSize == 0) {return;}
	
	glUniformMatrix4fv(R_Uniforms.ModelMats, R_State.BatchSize, GL_FALSE, (f32 *)R_State.BatchModelMats);
	glUniformMatrix3fv(R_Uniforms.NormalMats, R_State.BatchSize, GL_FALSE, (f32 *)R_State.BatchNormalMats);
	glBindVertexArray(R_Models[R_M_PLANE].VAO);
	glDrawElementsInstanced(GL_TRIANGLES, R_Models[R_M_PLANE].IdxCnt, GL_UNSIGNED_INT, NULL, R_State.BatchSize);
	R_State.BatchSize = 0;
}

void
R_Update(void)
{
	// update fade brightness.
	if (R_State.FadeStatus)
	{
		R_State.FadeBrightness += O_FADE_SPEED;
		R_State.FadeBrightness = R_State.FadeBrightness > 1.0f ? 1.0f : R_State.FadeBrightness;
	}
	else
	{
		R_State.FadeBrightness -= O_FADE_SPEED;
		R_State.FadeBrightness = R_State.FadeBrightness < 0.0f ? 0.0f : R_State.FadeBrightness;
	}
}

void
R_Fade(R_FadeStatus FS)
{
	R_State.FadeStatus = FS;
}

static void
R_PreprocShader(char *Src, usize Len)
{
	// substitute all constants for in-game values prior to shader compilation.
	for (usize i = 0; i < Len; ++i)
	{
		if (Len - i >= 13 && !strncmp(&Src[i], "$O_MAX_LIGHTS", 13))
		{
			usize CL = snprintf(&Src[i], 13, "%u", O_MAX_LIGHTS);
			memset(&Src[i + CL], ' ', 13 - CL);
		}
		else if (Len - i >= 16 && !strncmp(&Src[i], "$O_AMBIENT_LIGHT", 16))
		{
			usize CL = snprintf(&Src[i], 16, "%f", O_AMBIENT_LIGHT);
			memset(&Src[i + CL], ' ', 16 - CL);
		}
		else if (Len - i >= 13 && !strncmp(&Src[i], "$O_LIGHT_STEP", 13))
		{
			usize CL = snprintf(&Src[i], 13, "%f", O_LIGHT_STEP);
			memset(&Src[i + CL], ' ', 13 - CL);
		}
		else if (Len - i >= 14 && !strncmp(&Src[i], "$O_SHADOW_BIAS", 14))
		{
			usize CL = snprintf(&Src[i], 14, "%f", O_SHADOW_BIAS);
			memset(&Src[i + CL], ' ', 14 - CL);
		}
		else if (Len - i >= 15 && !strncmp(&Src[i], "$O_CAM_CLIP_FAR", 15))
		{
			usize CL = snprintf(&Src[i], 15, "%f", O_CAM_CLIP_FAR);
			memset(&Src[i + CL], ' ', 15 - CL);
		}
		else if (Len - i >= 17 && !strncmp(&Src[i], "$O_MAX_TILE_BATCH", 17))
		{
			usize CL = snprintf(&Src[i], 17, "%u", O_MAX_TILE_BATCH);
			memset(&Src[i + CL], ' ', 17 - CL);
		}
	}
}

static void
R_DeleteGlContext(void)
{
	SDL_GL_DeleteContext(R_GlContext);
}
