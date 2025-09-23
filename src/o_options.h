// SPDX-License-Identifier: GPL-3.0-or-later

// program information.
#define O_VERSION "1.0.1"

// windowing and setup options.
#define O_SDLFLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define O_IMGFLAGS IMG_INIT_PNG
#define O_MIXFLAGS MIX_INIT_OGG
#define O_WNDTITLE "Shame on Me"
#define O_WNDFLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED)
#define O_ERRWNDTITLE "Shame on Me - Error"

// game system options.
#define O_TICKMS 20
#define O_TICKUS (1000 * O_TICKMS)
#define O_MAXACTIONS 512
#define O_MAXPROPS 64
#define O_MAXLOGLEN 512
#define O_MAXUIELEMS 32

// game visual options.
#define O_VERTBOB 0.2f
#define O_HORIZBOB 0.1f
#define O_BOBFREQ 0.1f
#define O_WALKSPEED 0.08f
#define O_LOOKSPEED 0.4f
#define O_TEXTBOXSIZE 70
#define O_TEXTSPRITESIZE 100
#define O_FADESPEED 0.005f
#define O_PANPOSSPEED 0.1f
#define O_PANROTSPEED 0.1f
#define O_CARDWIDTH 55
#define O_CARDHEIGHT 80
#define O_CARDOUTLINE 1
#define O_CARDSPEED 0.2f
#define O_TEXTSCROLL 1
#define O_VOICETICK 10
#define O_BASEGLOBALSHADE 1.0f, 0.9f, 0.8f
#define O_DEATHGLOBALSHADE 1.0f, 0.0f, 0.0f
#define O_CLEARCOLOR 0.0f, 0.0f, 0.0f
#define O_QUAKESHAKE 2.5f
#define O_DEATHSHAKE 1.0f
#define O_PLAYERDEATHSHAKE 4.0f
#define O_MENUCYCLERATE 0.006f
#define O_MENUCYCLEBOB 0.1f
#define O_HIGHPIXELATION 3.0f
#define O_LOWPIXELATION 1.5f
#define O_MENUTEXTHEIGHT 16
#define O_SHAKEATTEN 0.99f
#define O_SHAKEPOS 0.6f
#define O_SHAKEROT 0.5f
#define O_SHAKEPOSRET 0.93f
#define O_SHAKEROTRET 0.95f

// UI visual options.
#define O_UIPANELPAD 6
#define O_UIBUTTONPAD 2
#define O_UISLIDERPAD 2

// sound options.
#define O_MUSICFADE 0.01f

// render options.
#ifdef DYNAMICLIGHTS
#define O_AMBIENTLIGHT 0.05f
#else
#define O_AMBIENTLIGHT 1.0f
#endif
#define O_MAXLIGHTS 4
#define O_LIGHTSTEP 0.1f
#define O_SHADOWBIAS 0.15f
#define O_CAMFOV (GLM_PI / 2.5f)
#define O_CAMCLIPNEAR 0.1f
#define O_CAMCLIPFAR 500.0f
#define O_FONTSIZE 14
#define O_FONTCOLOR 255, 255, 255
#define O_SHADOWMAPSIZE 1024
#define O_MAXTILEBATCH 64

// durak options.
#define O_ATTACKTICK 50
#define O_DEFENDTICK 30
#define O_VARYTICK 50
#define O_DEATHLOSSES 2

// dynamic configuration.
#define O_DYNFILE "options.conf"

typedef struct o_dyn
{
	i64 wndw, wndh;
	SDL_Keycode ksel, kskip, kright, kleft;
	f64 sfxvolume, musicvolume;
	f64 pixelation;
	bool fullscreen;
} o_dyn_t;

extern o_dyn_t o_dyn;

void o_dyndefault(void);
i32 o_dynread(void);
i32 o_dynwrite(void);
