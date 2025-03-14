#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define MAP \
	"S.....A..." \
	"..####e###" \
	"a.....B.C." \
	".........D" \
	".........." \
	"b..H.....E" \
	"....G...F." \
	"..c...d..." \
	".........." \
	"......f..."

void
G_Loop(void)
{
	g_Map = (struct Map)
	{
		.Data = MAP,
		.w = MAP_WIDTH,
		.h = MAP_HEIGHT
	};
	
	//R_Light((vec3){5.0f, 0.0f, 5.0f}, 1.1f);
	R_Light((vec3){6.0f, 0.0f, 0.0f}, 1.3f);
	R_Light((vec3){8.0f, -0.6f, 3.0f}, 0.3f);
	
	C_LookWalkTo(A_ARKADY, 'A');
	C_Wait(300);
	C_LookAt(A_ARKADY, 'S');
	C_Wait(200);
	C_LookWalkTo(A_ARKADY, 'B');
	C_WalkTo(A_PETER, 'a');
	C_WalkTo(A_PETER, 'b');
	C_WalkTo(A_PETER, 'c');
	C_WalkTo(A_PETER, 'd');
	C_Speak(TS_DUMMY, "Welcome... To the Bog...");
	C_LookWalkTo(A_ARKADY, 'C');
	C_LookWalkTo(A_ARKADY, 'D');
	C_LookWalkTo(A_ARKADY, 'E');
	C_LookWalkTo(A_ARKADY, 'F');
	C_LookWalkTo(A_ARKADY, 'G');
	C_LookWalkTo(A_ARKADY, 'H');
	C_LookAt(A_ARKADY, 'A');
	C_WalkTo(A_PETER, 'e');
	C_Wait(200);
	C_LookAt(A_ARKADY, 'f');
	C_WalkTo(A_PETER, 'f');
	C_LookAt(A_ARKADY, 'e');
	C_WalkTo(A_PETER, 'D');
	C_WalkTo(A_MATTHEW, 'A');
	C_WalkTo(A_MATTHEW, 'B');
	C_WalkTo(A_MATTHEW, 'C');
	C_WalkTo(A_GERASIM, 'A');
	C_WalkTo(A_GERASIM, 'B');
	
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
		T_Update();
		
		// render.
		R_SetShaderProgram(SP_SHADOW);
		for (usize i = 0; i < CF_MAX_LIGHTS; ++i)
		{
			R_BeginShadow(i);
			C_Render();
		}
		
		R_SetShaderProgram(SP_BASE);
		R_BeginFrame();
		C_Render();
		
		R_SetShaderProgram(SP_OVERLAY);
		T_Render();
		
		R_Present();
		
		EndTick();
	}
}
