#ifndef CONF_H
#define CONF_H

// windowing and setup options.
#define CF_SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define CF_IMG_INIT_FLAGS IMG_INIT_PNG
#define CF_WND_TITLE "Shame on Me"
#define CF_WND_FLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED)
#define CF_ERR_WND_TITLE "Shame on Me - Error"

// input options.
#define CF_KEY_NEXT SDLK_c

// game system options.
#define CF_TICK_MS 20
#define CF_MAX_CHOREO_ACTIONS 256
#define CF_MAX_LOG_LEN 512

// non-render stylistic options.
#define CF_VERT_BOB_INTENSITY 0.2f
#define CF_HORIZ_BOB_INTENSITY 0.1f
#define CF_BOB_FREQUENCY 0.1f
#define CF_WALK_SPEED 0.08f
#define CF_LOOK_SPEED 0.25f

// render options.
#define CF_MAX_LIGHTS 4
#define CF_AMBIENT_LIGHT 0.05f
#define CF_LIGHT_STEP 0.1f
#define CF_SHADOW_BIAS 0.4f
#define CF_PIXELATION 5.0f
#define CF_CAM_FOV (GLM_PI / 2.5f)
#define CF_CAM_CLIP_NEAR 0.1f
#define CF_CAM_CLIP_FAR 500.0f
#define CF_FONT_PT 12
#define CF_SHADOW_MAP_SIZE 512

#endif
