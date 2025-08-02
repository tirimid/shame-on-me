// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum c_actor
{
	C_ARKADY = 0,
	C_PETER,
	C_MATTHEW,
	C_GERASIM,
	
	C_ACTOR_END
} c_actor_t;

typedef struct c_mapdata
{
	char const *data;
	u32 w, h;
} c_map_t;

typedef struct c_prop
{
	vec3 pos, rot, scale;
	u8 model;
	u8 tex;
} c_prop_t;

extern c_map_t c_map;
extern c_prop_t c_props[O_MAXPROPS];
extern usize c_nprops;

void c_reset(void);
void c_teleport(c_actor_t a, vec2 pos);
void c_teleportto(c_actor_t a, char point);
void c_walk(c_actor_t a, vec2 pos);
void c_walkto(c_actor_t a, char point);
void c_look(c_actor_t a, f32 pitchdeg, f32 yawdeg);
void c_lookat(c_actor_t a, char point);
void c_lookwalkto(c_actor_t a, char point);
void c_settexture(c_actor_t a, r_tex_t t);
void c_wait(u64 ms);
void c_speak(t_sprite_t t, char const *msg);
void c_swapmodel(usize idx, r_model_t newmodel);
void c_setlightintensity(usize idx, f32 intensity);
void c_pancamera(vec3 pos, f32 pitchdeg, f32 yawdeg);
void c_setdurakphase(d_phase_t phase);
void c_quit(void);
void c_playsfx(s_sfx_t sfx);
void c_globalshade(vec3 shade);
void c_shakecamera(f32 intensity);
void c_fade(r_fadestatus_t fs);
void c_setflag(bool *flag);
void c_update(void);
void c_rendertiles(void);
void c_rendermodels(void);
i64 c_putprop(r_model_t m, r_tex_t t, vec3 pos, vec3 rot, vec3 scale);
i64 c_putpropat(r_model_t m, r_tex_t t, char point, vec3 trans, vec3 rot, vec3 scale);
i32 c_putlightat(char point, vec3 trans, f32 intensity);
