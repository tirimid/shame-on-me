#ifndef OPTIONS_H
#define OPTIONS_H

// windowing and setup options.
#define O_SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define O_IMG_INIT_FLAGS IMG_INIT_PNG
#define O_WND_TITLE "Shame on Me"
#define O_WND_FLAGS (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED)
#define O_WND_WIDTH 800
#define O_WND_HEIGHT 450
#define O_ERR_WND_TITLE "Shame on Me - Error"

// input options.
#define O_KEY_NEXT SDLK_c

// game system options.
#define O_TICK_MS 20
#define O_MAX_CHOREO_ACTIONS 256
#define O_MAX_CHOREO_PROPS 64
#define O_MAX_LOG_LEN 512

// stylistic options.
#define O_VERT_BOB_INTENSITY 0.2f
#define O_HORIZ_BOB_INTENSITY 0.1f
#define O_BOB_FREQUENCY 0.1f
#define O_WALK_SPEED 0.08f
#define O_LOOK_SPEED 0.25f
#define O_TEXT_BOX_SIZE 70
#define O_TEXT_SPRITE_SIZE 60
#define O_FADE_SPEED 0.005f

// render options.
#define O_MAX_LIGHTS 4
#define O_AMBIENT_LIGHT 0.05f
#define O_LIGHT_STEP 0.1f
#define O_SHADOW_BIAS 0.4f
#define O_PIXELATION 5.0f
#define O_CAM_FOV (GLM_PI / 2.5f)
#define O_CAM_CLIP_NEAR 0.1f
#define O_CAM_CLIP_FAR 500.0f
#define O_FONT_PT 16
#define O_FONT_COLOR {255, 255, 255, 255}
#define O_SHADOW_MAP_SIZE 512
#define O_MAX_PLANE_BATCH 128

#endif
