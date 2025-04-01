#ifndef CHOREO_H
#define CHOREO_H

typedef enum C_Actor
{
	C_A_ARKADY = 0,
	C_A_PETER,
	C_A_MATTHEW,
	C_A_GERASIM,
	
	C_A_END__
} C_Actor;

typedef struct C_MapData
{
	char const *Data;
	u32 w, h;
} C_MapData;

typedef struct C_Prop
{
	vec3 Pos, Rot, Scale;
	u8 Model;
	u8 Tex;
} C_Prop;

extern C_MapData C_Map;
extern C_Prop C_Props[O_MAX_CHOREO_PROPS];
extern usize C_PropCnt;

void C_Teleport(C_Actor a, vec2 Pos);
void C_TeleportTo(C_Actor a, char Point);
void C_Walk(C_Actor a, vec2 Pos);
void C_WalkTo(C_Actor a, char Point);
void C_Look(C_Actor a, f32 PitchDeg, f32 YawDeg);
void C_LookAt(C_Actor a, char Point);
void C_LookWalkTo(C_Actor a, char Point);
void C_SetTexture(C_Actor a, R_Texture t);
void C_Wait(u64 MS);
void C_Speak(T_TextboxSprite t, char const *Msg);
void C_SwapModel(usize Idx, R_Model NewModel);
void C_SetLightIntensity(usize Idx, f32 Intensity);
void C_Update(void);
void C_RenderTiles(void);
void C_RenderModels(void);
i64 C_PutProp(R_Model m, R_Texture t, vec3 Pos, vec3 Rot, vec3 Scale);

#endif
