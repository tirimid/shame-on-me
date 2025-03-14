#ifndef RENDER_H
#define RENDER_H

enum Model
{
	M_PLANE = 0,
	M_CUBE,
	
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
	
	T_END__
};

enum Font
{
	F_VCR_OSD_MONO = 0,
	
	F_END__
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
void R_Model(enum Model m, enum Texture t, vec3 Pos, vec3 Rot, vec3 Scale);
void R_Light(vec3 Pos, f32 Intensity);
void R_Rect(enum Texture t, i32 x, i32 y, i32 w, i32 h);
void R_Text(enum Font f, char const *Text, i32 x, i32 y, i32 w, i32 h);

#endif
