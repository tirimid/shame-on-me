#ifndef CHOREO_H
#define CHOREO_H

typedef enum c_actor
{
	C_ARKADY = 0,
	C_PETER,
	C_MATTHEW,
	C_GERASIM,
	
	C_ACTOR_END
} c_actor;

typedef struct c_mapdata
{
	char const *data;
	u32 w, h;
} c_mapdata;

typedef struct c_prop
{
	vec3 pos, rot, scale;
	u8 model;
	u8 tex;
} c_prop;

extern c_mapdata c_map;
extern c_prop c_props[O_MAXPROPS];
extern usize c_propcnt;

void c_teleport(c_actor a, vec2 pos);
void c_teleportto(c_actor a, char point);
void c_walk(c_actor a, vec2 pos);
void c_walkto(c_actor a, char point);
void c_look(c_actor a, f32 pitchdeg, f32 yawdeg);
void c_lookat(c_actor a, char point);
void c_lookwalkto(c_actor a, char point);
void c_settexture(c_actor a, r_tex t);
void c_wait(u64 ms);
void c_speak(t_sprite t, char const *msg);
void c_swapmodel(usize idx, r_model newmodel);
void c_setlightintensity(usize idx, f32 intensity);
void c_pancamera(vec3 pos, f32 pitchdeg, f32 yawdeg);
void c_setdurakphase(d_gamephase phase);
void c_update(void);
void c_rendertiles(void);
void c_rendermodels(void);
i64 c_putprop(r_model m, r_tex t, vec3 pos, vec3 rot, vec3 scale);

#endif
