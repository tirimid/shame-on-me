// SPDX-License-Identifier: GPL-3.0-or-later

// windowing and setup options.
#define O_SDLFLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define O_IMGFLAGS IMG_INIT_PNG
#define O_MIXFLAGS MIX_INIT_OGG
#define O_WNDTITLE "Shame on Me"
#define O_WNDFLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP)
#define O_WNDWIDTH 800
#define O_WNDHEIGHT 450
#define O_ERRWNDTITLE "Shame on Me - Error"

// input options.
#define O_KSEL SDLK_c
#define O_KSKIP SDLK_z
#define O_KRIGHT SDLK_RIGHT
#define O_KLEFT SDLK_LEFT

// game system options.
#define O_TICKMS 20
#define O_TICKUS (1000 * O_TICKMS)
#define O_MAXACTIONS 512
#define O_MAXPROPS 64
#define O_MAXLOGLEN 512

// stylistic options.
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

// render options.
#ifdef DYNAMICLIGHTS
#define O_AMBIENTLIGHT 0.05f
#else
#define O_AMBIENTLIGHT 1.0f
#endif
#define O_MAXLIGHTS 4
#define O_LIGHTSTEP 0.1f
#define O_SHADOWBIAS 0.15f
#define O_PIXELATION 4.0f
#define O_CAMFOV (GLM_PI / 2.5f)
#define O_CAMCLIPNEAR 0.1f
#define O_CAMCLIPFAR 500.0f
#define O_FONTSIZE 14
#define O_FONTCOLOR {255, 255, 255, 255}
#define O_SHADOWMAPSIZE 1024
#define O_MAXTILEBATCH 64
#define O_GLOBALSHADE "1.0,0.9,0.8"

// durak options.
#define O_ATTACKTICK 50
#define O_DEFENDTICK 30
#define O_VARYTICK 50

// sound options.
#define O_VOLUME 0.4f
