// SPDX-License-Identifier: GPL-3.0-or-later

#define G_MAPWIDTH 17
#define G_MAPHEIGHT 15
#define G_MAP \
	".............A..#" \
	"#############.B.#" \
	"........#...#...#" \
	"........#...#...#" \
	"........#...#...e" \
	"##########d##...#" \
	"...........E#...#" \
	"h..F......D.c.C.#" \
	".........g..#...#" \
	"###b#########...#" \
	"...H..K...#.#.f.#" \
	"..........#.#####" \
	"....L.a.J.#......" \
	"..........#......" \
	"......I...#......"

static void g_setupenv(void);

static usize g_maindoor, g_roomdoor;
static usize g_hallwaylight, g_entrylight, g_roomlight;

void
g_loop(void)
{
	(void)g_fastintroseq; // TODO: remove temporary warning suppression.
	
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	g_setupenv();
	g_introseq();
	g_tutorialseq();
	
	for (;;)
	{
		begintick();
		
		// handle events.
		i_prepare();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					r_resize(e.window.data1, e.window.data2);
				}
				break;
			case SDL_KEYDOWN:
				i_setkstate(&e, I_PRESS);
				break;
			case SDL_KEYUP:
				i_setkstate(&e, I_RELEASE);
				break;
			case SDL_QUIT:
				return;
			default:
				break;
			}
		}
		
		// update.
		c_update();
		d_update();
		r_update();
		
		// render.
		BEGINTIMER(&largetimer);
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_SHADOW);
		for (usize i = 0; i < O_MAXLIGHTS; ++i)
		{
			if (!r_lightenabled(i))
			{
				continue;
			}
			r_beginshadow(i);
			c_rendertiles();
			c_rendermodels();
		}
		ENDTIMER(stagetimer, "game: render shadow");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_BASE);
		r_beginbase();
		c_rendertiles();
		c_rendermodels();
		ENDTIMER(stagetimer, "game: render base");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		d_renderoverlay();
		t_renderoverlay();
		ENDTIMER(stagetimer, "game: render overlay");
		
		ENDTIMER(largetimer, "game: render");
		
		r_present();
		
		// update textbox after render in order to avoid effect where it disappears
		// for one frame before reappearing on the next dialog.
		t_update();
		
		endtick();
	}
}

static void
g_setupenv(void)
{
	// set map.
	c_map = (c_map_t)
	{
		.data = G_MAP,
		.w = G_MAPWIDTH,
		.h = G_MAPHEIGHT
	};
	
	// place lights.
	g_hallwaylight = c_putlightat('C', (vec3){0.0f, 0.0f, 0.0f}, 1.5f);
	g_entrylight = c_putlightat('g', (vec3){0.0f, 0.0f, 0.0f}, 0.0f);
	g_roomlight = c_putlightat('a', (vec3){0.0f, 0.0f, 0.0f}, 0.0f);
	
	// place props.
	g_maindoor = c_putpropat(
		R_DOORCLOSED,
		R_DOOR,
		'c',
		(vec3){0.0f, -1.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putpropat(
		R_DOORCLOSED,
		R_DOOR,
		'e',
		(vec3){0.0f, -1.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	g_roomdoor = c_putpropat(
		R_DOORCLOSED,
		R_DOOR,
		'b',
		(vec3){0.0f, -1.5f, 0.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putpropat(
		R_DOORCLOSED,
		R_DOOR,
		'd',
		(vec3){0.0f, -1.5f, 0.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'f',
		(vec3){0.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'f',
		(vec3){-0.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'h',
		(vec3){-0.5f, -0.3f, -0.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'h',
		(vec3){-0.5f, -0.3f, 0.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MTABLE,
		R_TTABLE,
		'a',
		(vec3){0.0f, -1.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.2f, 0.4f, 1.2f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'I',
		(vec3){-0.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'I',
		(vec3){-1.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'I',
		(vec3){-2.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		'C',
		(vec3){0.0f, 0.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	c_putpropat(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		'g',
		(vec3){0.0f, 0.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	c_putpropat(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		'a',
		(vec3){0.0f, 0.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
}

void
g_introseq(void)
{
	r_fade(R_FADEIN);
	
	c_lookwalkto(C_ARKADY, 'A');
	c_lookwalkto(C_ARKADY, 'B');
	c_lookat(C_ARKADY, 'C');
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_ARKADY, "\"I dread that this'll be what I thought\"");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_ARKADY, "\"But I've already arrived\"");
	c_speak(T_ARKADY, "\"It's too late to leave\"");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_ARKADY, "\"There's no turning back now\"");
	c_wait(300);
	c_walkto(C_ARKADY, 'C');
	c_lookat(C_ARKADY, 'D');
	c_wait(1500);
	c_speak(T_MUTEARKADY, "...");
	c_wait(2500);
	c_teleportto(C_GERASIM, 'D');
	c_setlightintensity(g_entrylight, 1.5f);
	c_swapmodel(g_maindoor, R_DOOROPEN);
	c_wait(400);
	c_speak(T_MUTEGERASIM, "...");
	c_speak(T_MUTEGERASIM, "Gerasim stares at you regretfully");
	c_wait(1500);
	c_speak(T_ARKADY, "\"It's good to see you again\"");
	c_speak(T_MUTEGERASIM, "...");
	c_walkto(C_GERASIM, 'E');
	c_walkto(C_ARKADY, 'D');
	c_swapmodel(g_maindoor, R_DOORCLOSED);
	c_setlightintensity(g_hallwaylight, 0.0f);
	c_lookat(C_ARKADY, 'E');
	c_speak(T_ARKADY, "\"Are the others already here?\"");
	c_speak(T_MUTEGERASIM, "He nods with difficulty");
	c_speak(T_ARKADY, "\"I see\"");
	c_wait(200);
	c_lookwalkto(C_ARKADY, 'F');
	c_lookat(C_ARKADY, 'H');
	c_wait(800);
	c_teleportto(C_MATTHEW, 'I');
	c_teleportto(C_PETER, 'J');
	c_setlightintensity(g_roomlight, 1.5f);
	c_swapmodel(g_roomdoor, R_DOOROPEN);
	c_wait(200);
	c_walkto(C_ARKADY, 'H');
	c_swapmodel(g_roomdoor, R_DOORCLOSED);
	c_setlightintensity(g_entrylight, 0.0f);
	c_lookat(C_ARKADY, 'J');
	c_wait(400);
	c_lookat(C_ARKADY, 'I');
	c_wait(250);
	c_speak(T_MATTHEW, "\"Oh... You're already here, Arkady?\"");
	c_speak(T_MUTEMATTHEW, "Matthew's face looks about as friendly as it usually does");
	c_wait(300);
	c_speak(T_ARKADY, "\"That's right\"");
	c_speak(T_MUTEARKADY, "(Act friendly, maybe I'm wrong, maybe it's not what I thought)");
	c_speak(T_PETER, "\"Look, I'm sorry I didn't tell you why we needed you here\"");
	c_lookat(C_ARKADY, 'J');
	c_wait(100);
	c_speak(T_MUTEPETER, "Peter's eyes reflect the room's light in a particularly cynical way");
	c_speak(T_PETER, "\"But that's kind of just the nature of the matter\"");
	c_speak(T_PETER, "\"If I'd been honest, you wouldn't have come\"");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_ARKADY, "\"Why did you call me over?\"");
	c_speak(T_PETER, "\"You're aware of what's happening with the Earth's crust, right? The earthquakes?\"");
	c_speak(T_MUTEARKADY, "(I knew it)");
	c_speak(T_ARKADY, "\"That's right\"");
	c_speak(T_PETER, "\"Good. It's all over the news, you'd have to be real dim not to notice the apocalypse\"");
	c_wait(2000);
	c_speak(T_PETER, "\"Anyway...\"");
	c_speak(T_PETER, "\"With the country falling apart, the government can't sustain our population anymore\"");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_PETER, "\"We have one day to decide who will die and who will live\"");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_PETER, "\"Or they will decide for us\"");
	c_wait(2500);
	c_speak(T_ARKADY, "\"So you've brought me here like a lamb to the slaughterhouse?\"");
	c_speak(T_ARKADY, "\"You're going to kill me so you can get off unscathed?\"");
	c_speak(T_PETER, "\"Not quite, Arkady, we're going to give you a chance\"");
	c_speak(T_ARKADY, "\"I don't understand\"");
	c_wait(500);
	c_speak(T_PETER, "\"We're going to play some special 'high stakes' cards\"");
	c_speak(T_MUTEARKADY, "(How sick)");
	c_speak(T_ARKADY, "\"I don't suppose I have much of a choice\"");
	c_wait(800);
	c_speak(T_PETER, "\"No, Arkady, you don't have a choice\"");
	c_wait(1500);
	c_speak(T_MATTHEW, "\"Well then, take a seat\"");
	c_wait(600);
	c_lookwalkto(C_ARKADY, 'K');
	c_lookat(C_ARKADY, 'I');
	c_teleportto(C_GERASIM, 'F');
	c_wait(400);
	c_walkto(C_GERASIM, 'H');
	c_lookat(C_ARKADY, 'H');
	c_wait(600);
	c_speak(T_ARKADY, "\"Come play with us, Gerasim\"");
	c_speak(T_MUTEGERASIM, "He gestures as if to indicate the obviousness of such an action");
	c_walkto(C_GERASIM, 'L');
	c_lookat(C_ARKADY, 'L');
	c_wait(500);
	c_lookat(C_ARKADY, 'I');
	c_wait(500);
	c_lookat(C_ARKADY, 'J');
	c_wait(500);
	c_lookat(C_ARKADY, 'I');
	c_speak(T_ARKADY, "\"So, what are we going to play?\"");
	c_speak(T_MATTHEW, "\"Do you know how to play Fool?\"");
	c_speak(T_ARKADY, "\"No, I can't say I do...\"");
	c_speak(T_MATTHEW, "\"That's alright, we'll show you how to play a simplified variant\"");
	c_wait(800);
	c_speak(T_MATTHEW, "\"Well then, shall we start?\"");
}

void
g_fastintroseq(void)
{
	r_fade(R_FADEIN);
	
	c_setlightintensity(g_hallwaylight, 0.0f);
	c_setlightintensity(g_entrylight, 0.0f);
	c_teleportto(C_ARKADY, 'K');
	c_teleportto(C_PETER, 'J');
	c_teleportto(C_MATTHEW, 'I');
	c_teleportto(C_GERASIM, 'L');
	c_lookat(C_ARKADY, 'I');
	c_wait(2000);
	c_setlightintensity(g_roomlight, 1.5f);
	c_wait(80);
	c_setlightintensity(g_roomlight, 0.6f);
	c_wait(100);
	c_setlightintensity(g_roomlight, 1.1f);
	c_wait(600);
	c_setlightintensity(g_roomlight, 1.5f);
	c_wait(500);
	c_speak(T_MATTHEW, "\"Well then, shall we start?\"");
}

void
g_tutorialseq(void)
{
	c_pancamera((vec3){0.0f, 0.0f, 2.0f}, -90.0f, 0.0f);
	c_wait(1200);
	c_setdurakphase(D_START);
	c_wait(400);
	c_speak(T_MATTHEW, "\"This is a special deck, I've gone and removed some cards\"");
	c_speak(T_MATTHEW, "\"There are 36 cards - only 6-10s, face cards, and aces\"");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "\"The first step is to pick out a trump suit\"");
	c_speak(T_MATTHEW, "\"We'll pick out a card and select its suit\"");
	c_speak(T_MATTHEW, "\"Be sure to remember it\"");
	c_wait(300);
	c_setdurakphase(D_CHOOSETRUMP);
	c_wait(400);
	c_speak(T_MATTHEW, "\"Next we have to deal out 6 cards to each player\"");
	c_wait(300);
	c_setdurakphase(D_DEALCARDS);
	c_wait(400);
	c_speak(T_ARKADY, "\"So, how do we play?\"");
	c_speak(T_MATTHEW, "\"Fool is a game where you need to get rid of your held cards\"");
	c_speak(T_MATTHEW, "\"On my turn, I'll 'attack' Gerasim, the player to my left, by laying out a card of my choice\"");
	c_speak(T_MATTHEW, "\"He must then cover them by playing a card of higher value...\"");
	c_speak(T_MATTHEW, "\"That's...\"");
	c_speak(T_MATTHEW, "\"Basically, card values ascend from 6 to 10, then Jack, Queen, King, and finally Ace\"");
	c_speak(T_MATTHEW, "\"And trump-suit cards are stronger than non-trump-suit cards\"");
	c_speak(T_MATTHEW, "\"So, for example, if the trump suit is Clubs, then a 6 of Clubs would be better than an Ace of Diamonds...\"");
	c_speak(T_MATTHEW, "\"But then a 7 of Clubs is still better than a 6...\"");
	c_speak(T_MATTHEW, "\"Other than that, only cards of equivalent suit can be played against each other\"");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "\"Uh...\"");
	c_speak(T_MATTHEW, "\"Right, then, when Gerasim covers my cards, I can give him more cards to cover\"");
	c_speak(T_MATTHEW, "\"But I can only play new cards with the same face value as previously played cards\"");
	c_speak(T_MATTHEW, "\"If he covers everything and I don't want to or can't attack him anymore, the attack ends, and he begins the next attack, this time on you\"");
	c_speak(T_MATTHEW, "\"If he can't cover my cards, he has to take all of the cards in play, and he skips his attack\"");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "\"Oh, and we all have to draw back up to six cards if we have less\"");
	c_speak(T_MATTHEW, "\"At least until the draw pile is exhausted\"");
	c_speak(T_MATTHEW, "\"That make sense?\"");
	c_speak(T_ARKADY, "\"I think I'll understand more once we actually start playing\"");
	c_speak(T_MATTHEW, "\"Oh...\"");
	c_speak(T_MATTHEW, "\"Well... No problem, of course...\"");
	c_speak(T_MATTHEW, "\"Just remember - defend, attack left, shed your cards\"");
	c_speak(T_MATTHEW, "\"Last player who still has cards loses\"");
	c_speak(T_MUTEARKADY, "(Even though it's just a practice round, I can feel my heart beating, I'm not ready)");
	c_speak(T_ARKADY, "\"I understand\"");
	c_wait(1000);
	c_speak(T_MATTHEW, "\"You all ready?\"");
	c_speak(T_MUTEGERASIM, "Gerasim picks up his cards, he has clearly played before");
	c_speak(T_ARKADY, "\"I think so\"");
	c_speak(T_PETER, "\"What difference does it make?\"");
	c_speak(T_MATTHEW, "\"I suppose that's true\"");
	c_speak(T_MATTHEW, "\"Let's just get started\"");
	c_wait(500);
	c_setdurakphase(D_ATTACK);
}

void
g_posttutorialseq(void)
{
	c_wait(800);
	c_pancamera((vec3){0.0f, 0.0f, 0.0f}, 0.0f, 0.0f);
	c_wait(1200);
	c_speak(T_MATTHEW, "\"Since that was just a practice round, nothing happens\"");
	c_speak(T_MATTHEW, "\"Are the rules of the game clearer now, Arkady?\"");
	c_speak(T_ARKADY, "\"Yeah, I think so...\"");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "\"That's great, probably\"");
	
	// TODO: finish post-tutorial sequence.
}
