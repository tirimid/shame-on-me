#define MAP_WIDTH 17
#define MAP_HEIGHT 12
#define MAP \
	".............A..#" \
	"#############.B.#" \
	"........#...#...#" \
	"........#...#...#" \
	"........#...#...." \
	"###.######.##...#" \
	"...G.......E#...#" \
	"....F.....D...C.#" \
	"............#...#" \
	"#############...#" \
	"............#...#" \
	"............#...#"

static void IntroSeq(void);

static i32 MainDoorProp;

void
G_Loop(void)
{
	NEW_MICRO_TIMER(Timer);
	
	// set up environment.
	g_Map = (struct Map)
	{
		.Data = MAP,
		.w = MAP_WIDTH,
		.h = MAP_HEIGHT
	};
	R_PutLight((vec3){14.0f, 0.4f, 1.0f}, 1.5f);
	R_PutLight((vec3){14.0f, 0.4f, 7.0f}, 1.5f);
	R_PutLight((vec3){9.0f, 0.4f, 8.0f}, 1.5f);
	
	MainDoorProp = C_PutProp(
		M_DOOR_CLOSED,
		T_DOOR,
		(vec3){12.0f, -1.5f, 7.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		M_DOOR_CLOSED,
		T_DOOR,
		(vec3){16.0f, -1.5f, 4.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		M_DOOR_OPEN,
		T_DOOR,
		(vec3){3.0f, -1.5f, 5.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		M_DOOR_CLOSED,
		T_DOOR,
		(vec3){10.0f, -1.5f, 5.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	IntroSeq();
	
	for (;;)
	{
		BeginTick();
		
		// handle events.
		I_Prepare();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					R_HandleResize(e.window.data1, e.window.data2);
				break;
			case SDL_KEYDOWN:
				I_SetKeybdState(&e, IT_PRESS);
				break;
			case SDL_KEYUP:
				I_SetKeybdState(&e, IT_RELEASE);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		// update.
		C_Update();
		R_Update();
		
		// render.
		BEGIN_MICRO_TIMER(&Timer);
		R_SetShaderProgram(SP_SHADOW);
		for (usize i = 0; i < O_MAX_LIGHTS; ++i)
		{
			R_BeginShadow(i);
			C_Render();
		}
		END_MICRO_TIMER(Timer, "game: render shadows");
		
		BEGIN_MICRO_TIMER(&Timer);
		R_SetShaderProgram(SP_BASE);
		R_BeginBase();
		C_Render();
		END_MICRO_TIMER(Timer, "game: render base");
		
		BEGIN_MICRO_TIMER(&Timer);
		R_SetShaderProgram(SP_OVERLAY);
		R_BeginOverlay();
		if (T_IsActive())
			T_Render();
		END_MICRO_TIMER(Timer, "game: render overlay");
		
		R_Present();
		
		// update textbox after render in order to avoid effect where it disappears
		// for one frame before reappearing on the next dialog.
		T_Update();
		
		EndTick();
	}
}

static void
IntroSeq(void)
{
	R_Fade(FS_FADE_IN);
	
	C_LookWalkTo(A_ARKADY, 'A');
	C_LookWalkTo(A_ARKADY, 'B');
	C_LookAt(A_ARKADY, 'C');
	C_Speak(TS_ARKADY, "...");
	C_Speak(TS_ARKADY, "I dread that this'll be what I thought");
	C_Speak(TS_ARKADY, "...");
	C_Speak(TS_ARKADY, "But I've already arrived");
	C_Speak(TS_ARKADY, "It's too late to leave");
	C_Speak(TS_ARKADY, "...");
	C_Speak(TS_ARKADY, "There's no turning back now");
	C_Wait(300);
	C_WalkTo(A_ARKADY, 'C');
	C_LookAt(A_ARKADY, 'D');
	C_Wait(2500);
	C_Speak(TS_ARKADY, "(Knock knock knock)");
	C_Speak(TS_ARKADY, "...");
	C_Wait(1500);
	C_TeleportTo(A_GERASIM, 'D');
	C_SwapModel(MainDoorProp, M_DOOR_OPEN);
	C_Wait(400);
	C_Speak(TS_GERASIM, "...");
	C_Speak(TS_GERASIM, "(Gerasim looks at you quizzically, in an almost sad way)");
	C_Wait(1500);
	C_Speak(TS_ARKADY, "It's good to see you again");
	C_Speak(TS_GERASIM, "...");
	C_WalkTo(A_GERASIM, 'E');
	C_WalkTo(A_ARKADY, 'D');
	C_LookAt(A_ARKADY, 'E');
	C_Speak(TS_ARKADY, "Are the others already here?");
	C_Speak(TS_GERASIM, "(He nods weightfully)");
	C_Speak(TS_ARKADY, "I see");
	C_Wait(200);
	C_LookWalkTo(A_ARKADY, 'F');
	C_LookWalkTo(A_ARKADY, 'G');
}
