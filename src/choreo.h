#ifndef CHOREO_H
#define CHOREO_H

enum Actor
{
	A_ARKADY = 0,
	A_PETER,
	A_MATTHEW,
	A_GERASIM,
	
	A_END__
};

struct Map
{
	char const *Data;
	u32 w, h;
};

extern struct Map g_Map;

void C_Walk(enum Actor a, vec2 Pos);
void C_WalkTo(enum Actor a, char Point);
void C_Look(enum Actor a, f32 PitchDeg, f32 YawDeg);
void C_LookAt(enum Actor a, char Point);
void C_LookWalkTo(enum Actor a, char Point);
void C_SetTexture(enum Actor a, enum Texture t);
void C_Wait(u32 Ms);
void C_Speak(enum TextboxSprite Ts, char const *Msg);
void C_Update(void);
void C_Render(void);

#endif
