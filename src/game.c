#define G_MAP_WIDTH 17
#define G_MAP_HEIGHT 15
#define G_MAP \
	".............A..#" \
	"#############.B.#" \
	"........#...#...#" \
	"........#...#...#" \
	"........#...#...." \
	"##########.##...#" \
	"...........E#...#" \
	"...F......D...C.#" \
	"............#...#" \
	"###.#########...#" \
	"...H..K...#.#...#" \
	"..........#.#...#" \
	"....L...J.#.#...#" \
	"..........#.#...#" \
	"......I...#.#...#"

static void G_SetupEnvironment(void);
static void G_IntroSeq(void);
static void G_FastIntroSeq(void);

static usize G_MainDoorProp, G_RoomDoorProp;
static usize G_HallwayLight, G_EntryLight, G_RoomLight;

void
G_Loop(void)
{
	NEW_MICRO_TIMER(LargeTimer);
	NEW_MICRO_TIMER(StageTimer);
	
	G_SetupEnvironment();
	G_FastIntroSeq();
	
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
				{
					R_HandleResize(e.window.data1, e.window.data2);
				}
				break;
			case SDL_KEYDOWN:
				I_SetKeybdState(&e, I_IT_PRESS);
				break;
			case SDL_KEYUP:
				I_SetKeybdState(&e, I_IT_RELEASE);
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
		BEGIN_MICRO_TIMER(&LargeTimer);
		
		BEGIN_MICRO_TIMER(&StageTimer);
		R_SetShader(R_S_SHADOW);
		for (usize i = 0; i < O_MAX_LIGHTS; ++i)
		{
			if (!R_LightEnabled(i))
			{
				continue;
			}
			R_BeginShadow(i);
			C_RenderTiles();
			C_RenderModels();
			D_RenderCards();
		}
		END_MICRO_TIMER(StageTimer, "game: render shadow");
		
		BEGIN_MICRO_TIMER(&StageTimer);
		R_SetShader(R_S_BASE);
		R_BeginBase();
		C_RenderTiles();
		C_RenderModels();
		D_RenderCards();
		END_MICRO_TIMER(StageTimer, "game: render base");
		
		BEGIN_MICRO_TIMER(&StageTimer);
		R_SetShader(R_S_OVERLAY);
		R_BeginOverlay();
		T_RenderOverlay();
		D_RenderOverlay();
		END_MICRO_TIMER(StageTimer, "game: render overlay");
		
		END_MICRO_TIMER(LargeTimer, "game: render");
		
		R_Present();
		
		// update textbox after render in order to avoid effect where it disappears
		// for one frame before reappearing on the next dialog.
		T_Update();
		
		EndTick();
	}
}

static void
G_SetupEnvironment(void)
{
	// set map.
	C_Map = (C_MapData)
	{
		.Data = G_MAP,
		.w = G_MAP_WIDTH,
		.h = G_MAP_HEIGHT
	};
	
	// place lights.
	G_HallwayLight = R_PutLight((vec3){14.0f, 0.0f, 7.0f}, 1.5f);
	G_EntryLight = R_PutLight((vec3){9.0f, 0.0f, 8.0f}, 0.0f);
	G_RoomLight = R_PutLight((vec3){6.0f, 0.0f, 12.0f}, 0.0f);
	
	// place props.
	G_MainDoorProp = C_PutProp(
		R_M_DOOR_CLOSED,
		R_T_DOOR,
		(vec3){12.0f, -1.5f, 7.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		R_M_DOOR_CLOSED,
		R_T_DOOR,
		(vec3){16.0f, -1.5f, 4.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	G_RoomDoorProp = C_PutProp(
		R_M_DOOR_CLOSED,
		R_T_DOOR,
		(vec3){3.0f, -1.5f, 9.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		R_M_DOOR_CLOSED,
		R_T_DOOR,
		(vec3){10.0f, -1.5f, 5.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){14.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){13.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){-0.5f, -0.3f, 6.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){-0.5f, -0.3f, 7.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_TABLE,
		R_T_TABLE,
		(vec3){6.0f, -1.5f, 12.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.2f, 0.4f, 1.2f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){4.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){3.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_WINDOW,
		R_T_WINDOW,
		(vec3){5.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	C_PutProp(
		R_M_LIGHTBULB,
		R_T_LIGHTBULB,
		(vec3){14.0f, 0.5f, 7.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	C_PutProp(
		R_M_LIGHTBULB,
		R_T_LIGHTBULB,
		(vec3){9.0f, 0.5f, 8.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	C_PutProp(
		R_M_LIGHTBULB,
		R_T_LIGHTBULB,
		(vec3){6.0f, 0.5f, 12.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
}

static void
G_IntroSeq(void)
{
	R_Fade(R_FS_FADE_IN);
	
	C_LookWalkTo(C_A_ARKADY, 'A');
	C_LookWalkTo(C_A_ARKADY, 'B');
	C_LookAt(C_A_ARKADY, 'C');
	C_Speak(T_TS_ARKADY, "...");
	C_Speak(T_TS_ARKADY, "I dread that this'll be what I thought");
	C_Speak(T_TS_ARKADY, "...");
	C_Speak(T_TS_ARKADY, "But I've already arrived");
	C_Speak(T_TS_ARKADY, "It's too late to leave");
	C_Speak(T_TS_ARKADY, "...");
	C_Speak(T_TS_ARKADY, "There's no turning back now");
	C_Wait(300);
	C_WalkTo(C_A_ARKADY, 'C');
	C_LookAt(C_A_ARKADY, 'D');
	C_Wait(1500);
	C_Speak(T_TS_ARKADY, "...");
	C_Wait(2500);
	C_TeleportTo(C_A_GERASIM, 'D');
	C_SetLightIntensity(G_EntryLight, 1.5f);
	C_SwapModel(G_MainDoorProp, R_M_DOOR_OPEN);
	C_Wait(400);
	C_Speak(T_TS_GERASIM, "...");
	C_Speak(T_TS_GERASIM, "*Gerasim looks at you quizzically, in an almost sad way*");
	C_Wait(1500);
	C_Speak(T_TS_ARKADY, "It's good to see you again");
	C_Speak(T_TS_GERASIM, "...");
	C_WalkTo(C_A_GERASIM, 'E');
	C_WalkTo(C_A_ARKADY, 'D');
	C_SwapModel(G_MainDoorProp, R_M_DOOR_CLOSED);
	C_SetLightIntensity(G_HallwayLight, 0.0f);
	C_LookAt(C_A_ARKADY, 'E');
	C_Speak(T_TS_ARKADY, "Are the others already here?");
	C_Speak(T_TS_GERASIM, "*He nods weightfully*");
	C_Speak(T_TS_ARKADY, "I see");
	C_Wait(200);
	C_LookWalkTo(C_A_ARKADY, 'F');
	C_LookAt(C_A_ARKADY, 'H');
	C_Wait(800);
	C_TeleportTo(C_A_MATTHEW, 'I');
	C_TeleportTo(C_A_PETER, 'J');
	C_SetLightIntensity(G_RoomLight, 1.5f);
	C_SwapModel(G_RoomDoorProp, R_M_DOOR_OPEN);
	C_Wait(200);
	C_WalkTo(C_A_ARKADY, 'H');
	C_SwapModel(G_RoomDoorProp, R_M_DOOR_CLOSED);
	C_SetLightIntensity(G_EntryLight, 0.0f);
	C_LookAt(C_A_ARKADY, 'J');
	C_Wait(400);
	C_LookAt(C_A_ARKADY, 'I');
	C_Wait(250);
	C_Speak(T_TS_MATTHEW, "Oh... You're already here, Arkady?");
	C_Speak(T_TS_MATTHEW, "*Matthew's face radiates a kind of regretful melancholy*");
	C_Wait(300);
	C_Speak(T_TS_ARKADY, "That's right");
	C_Speak(T_TS_ARKADY, "(Act friendly, maybe I'm wrong, maybe it's not what I thought)");
	C_Speak(T_TS_PETER, "Look, I'm sorry I didn't tell you why we needed you here");
	C_LookAt(C_A_ARKADY, 'J');
	C_Wait(100);
	C_Speak(T_TS_PETER, "*Peter looks at you how you imagine a fox looks at his next meal*");
	C_Speak(T_TS_PETER, "But that's kind of just the nature of the matter");
	C_Speak(T_TS_PETER, "If I'd been honest, you wouldn't have come");
	C_Speak(T_TS_PETER, "...");
	C_Speak(T_TS_ARKADY, "Why did you call me over?");
	C_Speak(T_TS_PETER, "You're aware of the population crisis, right?");
	C_Speak(T_TS_ARKADY, "(I knew it)");
	C_Speak(T_TS_ARKADY, "That's right");
	C_Speak(T_TS_PETER, "Good. It's all over the news");
	C_Wait(2000);
	C_Speak(T_TS_PETER, "Anyway...");
	C_Speak(T_TS_PETER, "With the country falling apart, the government can't sustain our population anymore");
	C_Speak(T_TS_PETER, "...");
	C_Speak(T_TS_PETER, "We have one day to decide who will die and who will live");
	C_Speak(T_TS_PETER, "...");
	C_Speak(T_TS_PETER, "Or they will decide for us");
	C_Wait(2500);
	C_Speak(T_TS_ARKADY, "So you've brought me here like a lamb to the slaughterhouse?");
	C_Speak(T_TS_PETER, "Not quite, Arkady");
	C_Speak(T_TS_PETER, "Not the slaughterhouse, the casino");
	C_Speak(T_TS_ARKADY, "I don't understand");
	C_Wait(500);
	C_Speak(T_TS_PETER, "We're going to play cards - the loser dies");
	C_Speak(T_TS_ARKADY, "(How sick)");
	C_Speak(T_TS_ARKADY, "I don't suppose I have much of a choice");
	C_Wait(800);
	C_Speak(T_TS_PETER, "No, Arkady, you don't have a choice");
	C_Speak(T_TS_PETER, "Try to leave, and we'll just kill you on the spot");
	C_Wait(1500);
	C_Speak(T_TS_MATTHEW, "Well then, take a seat - our esteemed guest and friend");
	C_Wait(600);
	C_LookWalkTo(C_A_ARKADY, 'K');
	C_LookAt(C_A_ARKADY, 'I');
	C_TeleportTo(C_A_GERASIM, 'F');
	C_Wait(400);
	C_WalkTo(C_A_GERASIM, 'H');
	C_LookAt(C_A_ARKADY, 'H');
	C_Wait(600);
	C_Speak(T_TS_ARKADY, "Come play with us, Gerasim");
	C_Speak(T_TS_GERASIM, "*He gestures as if to indicate the obviousness of such an action*");
	C_WalkTo(C_A_GERASIM, 'L');
	C_LookAt(C_A_ARKADY, 'L');
	C_Wait(500);
	C_LookAt(C_A_ARKADY, 'I');
	C_Wait(500);
	C_LookAt(C_A_ARKADY, 'J');
	C_Wait(500);
	C_LookAt(C_A_ARKADY, 'I');
	C_Speak(T_TS_ARKADY, "So, what are we going to play?");
	C_Speak(T_TS_MATTHEW, "Do you know how to play Fool?");
	C_Speak(T_TS_ARKADY, "No, I can't say I do...");
	C_Speak(T_TS_MATTHEW, "That's alright, we'll show you how to play");
	C_Wait(800);
	C_Speak(T_TS_MATTHEW, "Well then, shall we start?");
	
	// TODO: work on normal intro sequence.
}

static void
G_FastIntroSeq(void)
{
	R_Fade(R_FS_FADE_IN);
	
	C_SetLightIntensity(G_HallwayLight, 0.0f);
	C_SetLightIntensity(G_EntryLight, 0.0f);
	C_TeleportTo(C_A_ARKADY, 'K');
	C_TeleportTo(C_A_PETER, 'J');
	C_TeleportTo(C_A_MATTHEW, 'I');
	C_TeleportTo(C_A_GERASIM, 'L');
	C_LookAt(C_A_ARKADY, 'I');
	C_Wait(2000);
	C_SetLightIntensity(G_RoomLight, 1.5f);
	C_Wait(80);
	C_SetLightIntensity(G_RoomLight, 0.6f);
	C_Wait(100);
	C_SetLightIntensity(G_RoomLight, 1.1f);
	C_Wait(600);
	C_SetLightIntensity(G_RoomLight, 1.5f);
	C_Wait(500);
	C_Speak(T_TS_MATTHEW, "Well then, shall we start?");
	C_PanCamera((vec3){0.0f, 0.8f, 1.4f}, -75.0f, 0.0f);
	C_Wait(1000);
}
