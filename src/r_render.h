// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum r_model
{
	R_PLANE = 0,
	R_CUBE,
	R_DOOROPEN,
	R_DOORCLOSED,
	R_MTABLE,
	R_MWINDOW,
	R_MLIGHTBULB,
	R_CARDSTACK,
	
	R_MODEL_END
} r_model_t;

typedef enum r_shader
{
	R_BASE = 0,
	R_OVERLAY,
	R_SHADOW,
	
	R_SHADER_END
} r_shader_t;

typedef enum r_tex
{
	R_SOMETHING = 0,
	R_FLOOR,
	R_CEILING,
	R_WALL,
	R_DUMMY,
	R_BLACK,
	R_DUMMYFACE,
	R_BLACK50,
	R_GLASSESDUMMY,
	R_GLASSESDUMMYFACE,
	R_CBACK,
	R_CS6,
	R_CS7,
	R_CS8,
	R_CS9,
	R_CS10,
	R_CSJ,
	R_CSQ,
	R_CSK,
	R_CSA,
	R_CD6,
	R_CD7,
	R_CD8,
	R_CD9,
	R_CD10,
	R_CDJ,
	R_CDQ,
	R_CDK,
	R_CDA,
	R_CC6,
	R_CC7,
	R_CC8,
	R_CC9,
	R_CC10,
	R_CCJ,
	R_CCQ,
	R_CCK,
	R_CCA,
	R_CH6,
	R_CH7,
	R_CH8,
	R_CH9,
	R_CH10,
	R_CHJ,
	R_CHQ,
	R_CHK,
	R_CHA,
	R_DOOR,
	R_TTABLE,
	R_TWINDOW,
	R_TLIGHTBULB,
	R_EYESDUMMY,
	R_EYESDUMMYFACE,
	R_GRAY,
	R_MATTHEWFACE,
	R_MATTHEW0,
	R_MATTHEW1,
	R_GERASIMFACE,
	R_GERASIM0,
	R_GERASIM1,
	R_PETERFACE,
	R_PETER0,
	R_PETER1,
	R_ARKADYFACE,
	R_ARKADY0,
	
	R_TEX_END
} r_tex_t;

typedef enum r_font
{
	R_VCROSDMONO = 0,
	
	R_FONT_END
} r_font_t;

typedef enum r_fadestatus
{
	R_FADEOUT = 0,
	R_FADEIN
} r_fadestatus_t;

typedef struct r_cam
{
	// handled by other modules.
	struct
	{
		vec3 pos;
		f32 pitch, yaw;
		f32 shake;
	} base;
	
	// handled by render.
	struct
	{
		vec3 pos;
		f32 pitch, yaw;
		vec3 dstpos;
		f32 dstpitch, dstyaw;
	} pan;
	
	// handled by render.
	struct
	{
		vec3 pos;
		f32 pitch, yaw;
	} shake;
} r_cam_t;

extern SDL_Window *r_wnd;
extern r_cam_t r_cam;

i32 r_init(void);
void r_handle(SDL_Event const *e);
bool r_lightenabled(usize idx);
void r_renderbounds(OUT i32 *w, OUT i32 *h);
void r_beginshadow(usize idx);
void r_beginbase(void);
void r_beginoverlay(void);
void r_present(void);
void r_resize(i32 x, i32 y);
void r_setshader(r_shader_t s);
void r_settex(r_tex_t t);
void r_rendermodel(r_model_t m, vec3 pos, vec3 rot, vec3 scale);
i32 r_putlight(vec3 pos, f32 intensity);
void r_setlightintensity(usize idx, f32 intensity);
void r_resetlights(void);
void r_renderrect(r_tex_t t, i32 x, i32 y, i32 w, i32 h, f32 angle);
void r_rendertext(r_font_t f, char const *text, i32 x, i32 y, i32 w, i32 h);
void r_batchtile(vec3 pos, vec3 rot, vec3 scale);
void r_flushtiles(void);
void r_update(void);
void r_fade(r_fadestatus_t fs);
void r_cut(r_fadestatus_t fs);
void r_pancam(vec3 pos, f32 pitch, f32 yaw);
void r_effcamstate(OUT vec3 pos, OUT f32 *pitch, OUT f32 *yaw);
void r_globalshade(vec3 shade);
void r_textsize(r_font_t f, char const *text, OUT i32 *x, OUT i32 *y);
