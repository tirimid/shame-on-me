#ifndef RENDER_H
#define RENDER_H

typedef enum R_Model
{
	R_M_PLANE = 0,
	R_M_CUBE,
	R_M_DOOR_OPEN,
	R_M_DOOR_CLOSED,
	
	R_M_END__
} R_Model;

typedef enum R_Shader
{
	R_S_BASE = 0,
	R_S_OVERLAY,
	R_S_SHADOW,
	
	R_S_END__
} R_Shader;

typedef enum R_Texture
{
	R_T_SOMETHING = 0,
	R_T_FLOOR,
	R_T_CEILING,
	R_T_WALL,
	R_T_DUMMY,
	R_T_BLACK,
	R_T_DUMMY_FACE,
	R_T_BLACK50,
	R_T_GLASSES_DUMMY,
	R_T_GLASSES_DUMMY_FACE,
	R_T_C_SPADES_6,
	R_T_C_SPADES_7,
	R_T_C_SPADES_8,
	R_T_C_SPADES_9,
	R_T_C_SPADES_10,
	R_T_C_SPADES_J,
	R_T_C_SPADES_Q,
	R_T_C_SPADES_K,
	R_T_C_SPADES_A,
	R_T_C_DIAMONDS_6,
	R_T_C_DIAMONDS_7,
	R_T_C_DIAMONDS_8,
	R_T_C_DIAMONDS_9,
	R_T_C_DIAMONDS_10,
	R_T_C_DIAMONDS_J,
	R_T_C_DIAMONDS_Q,
	R_T_C_DIAMONDS_K,
	R_T_C_DIAMONDS_A,
	R_T_C_CLUBS_6,
	R_T_C_CLUBS_7,
	R_T_C_CLUBS_8,
	R_T_C_CLUBS_9,
	R_T_C_CLUBS_10,
	R_T_C_CLUBS_J,
	R_T_C_CLUBS_Q,
	R_T_C_CLUBS_K,
	R_T_C_CLUBS_A,
	R_T_C_HEARTS_6,
	R_T_C_HEARTS_7,
	R_T_C_HEARTS_8,
	R_T_C_HEARTS_9,
	R_T_C_HEARTS_10,
	R_T_C_HEARTS_J,
	R_T_C_HEARTS_Q,
	R_T_C_HEARTS_K,
	R_T_C_HEARTS_A,
	R_T_DOOR,
	
	R_T_END__
} R_Texture;

typedef enum R_Font
{
	R_F_VCR_OSD_MONO = 0,
	
	R_F_END__
} R_Font;

typedef enum R_FadeStatus
{
	R_FS_FADE_OUT = 0,
	R_FS_FADE_IN
} R_FadeStatus;

typedef struct R_Camera
{
	vec3 Pos;
	f32 Pitch, Yaw;
} R_Camera;

extern R_Camera R_Cam;

i32 R_Init(void);
usize R_GetLightCnt(void);
void R_GetRenderBounds(i32 *OutW, i32 *OutH);
void R_BeginShadow(usize Idx);
void R_BeginBase(void);
void R_BeginOverlay(void);
void R_Present(void);
void R_HandleResize(i32 x, i32 y);
void R_SetShader(R_Shader s);
void R_SetTexture(R_Texture t);
void R_RenderModel(R_Model m, vec3 Pos, vec3 Rot, vec3 Scale);
i32 R_PutLight(vec3 Pos, f32 Intensity);
void R_RenderRect(R_Texture t, i32 x, i32 y, i32 w, i32 h);
void R_RenderText(R_Font f, char const *Text, i32 x, i32 y, i32 w, i32 h);
void R_BatchRenderPlane(vec3 Pos, vec3 Rot, vec3 Scale);
void R_FlushPlaneBatch(void);
void R_Update(void);
void R_Fade(R_FadeStatus FS);

#endif
