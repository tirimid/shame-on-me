#ifndef RENDER_H
#define RENDER_H

enum Model
{
	M_PLANE = 0,
	M_CUBE,
	M_DOOR_OPEN,
	M_DOOR_CLOSED,
	
	M_END__
};

enum ShaderProgram
{
	SP_BASE = 0,
	SP_OVERLAY,
	SP_SHADOW,
	
	SP_END__
};

enum Texture
{
	T_SOMETHING = 0,
	T_FLOOR,
	T_CEILING,
	T_WALL,
	T_DUMMY,
	T_BLACK,
	T_DUMMY_FACE,
	T_BLACK50,
	T_GLASSES_DUMMY,
	T_GLASSES_DUMMY_FACE,
	T_C_SPADES_6,
	T_C_SPADES_7,
	T_C_SPADES_8,
	T_C_SPADES_9,
	T_C_SPADES_10,
	T_C_SPADES_J,
	T_C_SPADES_Q,
	T_C_SPADES_K,
	T_C_SPADES_A,
	T_C_DIAMONDS_6,
	T_C_DIAMONDS_7,
	T_C_DIAMONDS_8,
	T_C_DIAMONDS_9,
	T_C_DIAMONDS_10,
	T_C_DIAMONDS_J,
	T_C_DIAMONDS_Q,
	T_C_DIAMONDS_K,
	T_C_DIAMONDS_A,
	T_C_CLUBS_6,
	T_C_CLUBS_7,
	T_C_CLUBS_8,
	T_C_CLUBS_9,
	T_C_CLUBS_10,
	T_C_CLUBS_J,
	T_C_CLUBS_Q,
	T_C_CLUBS_K,
	T_C_CLUBS_A,
	T_C_HEARTS_6,
	T_C_HEARTS_7,
	T_C_HEARTS_8,
	T_C_HEARTS_9,
	T_C_HEARTS_10,
	T_C_HEARTS_J,
	T_C_HEARTS_Q,
	T_C_HEARTS_K,
	T_C_HEARTS_A,
	T_DOOR,
	
	T_END__
};

enum Font
{
	F_VCR_OSD_MONO = 0,
	
	F_END__
};

enum FadeStatus
{
	FS_FADE_OUT = 0,
	FS_FADE_IN
};

struct Camera
{
	vec3 Pos;
	f32 Pitch, Yaw;
};

extern struct Camera g_Camera;

i32 R_Init(void);
usize R_GetLightCnt(void);
void R_GetRenderBounds(i32 *OutW, i32 *OutH);
void R_BeginShadow(usize Idx);
void R_BeginBase(void);
void R_BeginOverlay(void);
void R_Present(void);
void R_HandleResize(i32 x, i32 y);
void R_SetShaderProgram(enum ShaderProgram Prog);
void R_SetTexture(enum Texture t);
void R_RenderModel(enum Model m, vec3 Pos, vec3 Rot, vec3 Scale);
i32 R_PutLight(vec3 Pos, f32 Intensity);
void R_RenderRect(enum Texture t, i32 x, i32 y, i32 w, i32 h);
void R_RenderText(enum Font f, char const *Text, i32 x, i32 y, i32 w, i32 h);
void R_BatchRenderPlane(vec3 Pos, vec3 Rot, vec3 Scale);
void R_FlushPlaneBatch(void);
void R_Update(void);
void R_Fade(enum FadeStatus Fs);

#endif
