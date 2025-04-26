#define G_MAPWIDTH 17
#define G_MAPHEIGHT 15
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

static void g_setupenv(void);
static void g_intro(void);
static void g_fastintro(void);
static void g_tutorial(void);

static usize g_maindoor, g_roomdoor;
static usize g_hallwaylight, g_entrylight, g_roomlight;

void
g_loop(void)
{
	(void)g_intro; // TODO: remove temporary warning suppression.
	
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	g_setupenv();
	g_fastintro();
	g_tutorial();
	
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
	c_map = (c_mapdata)
	{
		.data = G_MAP,
		.w = G_MAPWIDTH,
		.h = G_MAPHEIGHT
	};
	
	// place lights.
	g_hallwaylight = r_putlight((vec3){14.0f, 0.0f, 7.0f}, 1.5f);
	g_entrylight = r_putlight((vec3){9.0f, 0.0f, 8.0f}, 0.0f);
	g_roomlight = r_putlight((vec3){6.0f, 0.0f, 12.0f}, 0.0f);
	
	// place props.
	g_maindoor = c_putprop(
		R_DOORCLOSED,
		R_DOOR,
		(vec3){12.0f, -1.5f, 7.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putprop(
		R_DOORCLOSED,
		R_DOOR,
		(vec3){16.0f, -1.5f, 4.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	g_roomdoor = c_putprop(
		R_DOORCLOSED,
		R_DOOR,
		(vec3){3.0f, -1.5f, 9.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putprop(
		R_DOORCLOSED,
		R_DOOR,
		(vec3){10.0f, -1.5f, 5.0f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){1.0f, 1.0f, 1.0f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){14.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){13.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){-0.5f, -0.3f, 6.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){-0.5f, -0.3f, 7.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MTABLE,
		R_TTABLE,
		(vec3){6.0f, -1.5f, 12.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.2f, 0.4f, 1.2f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){4.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){3.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MWINDOW,
		R_TWINDOW,
		(vec3){5.5f, -0.3f, 14.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.5f, 0.5f, 0.8f}
	);
	
	c_putprop(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		(vec3){14.0f, 0.5f, 7.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	c_putprop(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		(vec3){9.0f, 0.5f, 8.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	c_putprop(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		(vec3){6.0f, 0.5f, 12.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
}

static void
g_intro(void)
{
	r_fade(R_FADEIN);
	
	c_lookwalkto(C_ARKADY, 'A');
	c_lookwalkto(C_ARKADY, 'B');
	c_lookat(C_ARKADY, 'C');
	c_speak(T_ARKADY, "...");
	c_speak(T_ARKADY, "I dread that this'll be what I thought");
	c_speak(T_ARKADY, "...");
	c_speak(T_ARKADY, "But I've already arrived");
	c_speak(T_ARKADY, "It's too late to leave");
	c_speak(T_ARKADY, "...");
	c_speak(T_ARKADY, "There's no turning back now");
	c_wait(300);
	c_walkto(C_ARKADY, 'C');
	c_lookat(C_ARKADY, 'D');
	c_wait(1500);
	c_speak(T_ARKADY, "...");
	c_wait(2500);
	c_teleportto(C_GERASIM, 'D');
	c_setlightintensity(g_entrylight, 1.5f);
	c_swapmodel(g_maindoor, R_DOOROPEN);
	c_wait(400);
	c_speak(T_GERASIM, "...");
	c_speak(T_GERASIM, "*Gerasim looks at you quizzically, in an almost sad way*");
	c_wait(1500);
	c_speak(T_ARKADY, "It's good to see you again");
	c_speak(T_GERASIM, "...");
	c_walkto(C_GERASIM, 'E');
	c_walkto(C_ARKADY, 'D');
	c_swapmodel(g_maindoor, R_DOORCLOSED);
	c_setlightintensity(g_hallwaylight, 0.0f);
	c_lookat(C_ARKADY, 'E');
	c_speak(T_ARKADY, "Are the others already here?");
	c_speak(T_GERASIM, "*He nods weightfully*");
	c_speak(T_ARKADY, "I see");
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
	c_speak(T_MATTHEW, "Oh... You're already here, Arkady?");
	c_speak(T_MATTHEW, "*Matthew's face radiates a kind of regretful melancholy*");
	c_wait(300);
	c_speak(T_ARKADY, "That's right");
	c_speak(T_ARKADY, "(Act friendly, maybe I'm wrong, maybe it's not what I thought)");
	c_speak(T_PETER, "Look, I'm sorry I didn't tell you why we needed you here");
	c_lookat(C_ARKADY, 'J');
	c_wait(100);
	c_speak(T_PETER, "*Peter looks at you how you imagine a fox looks at his next meal*");
	c_speak(T_PETER, "But that's kind of just the nature of the matter");
	c_speak(T_PETER, "If I'd been honest, you wouldn't have come");
	c_speak(T_PETER, "...");
	c_speak(T_ARKADY, "Why did you call me over?");
	c_speak(T_PETER, "You're aware of the population crisis, right?");
	c_speak(T_ARKADY, "(I knew it)");
	c_speak(T_ARKADY, "That's right");
	c_speak(T_PETER, "Good. It's all over the news");
	c_wait(2000);
	c_speak(T_PETER, "Anyway...");
	c_speak(T_PETER, "With the country falling apart, the government can't sustain our population anymore");
	c_speak(T_PETER, "...");
	c_speak(T_PETER, "We have one day to decide who will die and who will live");
	c_speak(T_PETER, "...");
	c_speak(T_PETER, "Or they will decide for us");
	c_wait(2500);
	c_speak(T_ARKADY, "So you've brought me here like a lamb to the slaughterhouse?");
	c_speak(T_PETER, "Not quite, Arkady");
	c_speak(T_PETER, "Not the slaughterhouse, the casino");
	c_speak(T_ARKADY, "I don't understand");
	c_wait(500);
	c_speak(T_PETER, "We're going to play cards - the loser dies");
	c_speak(T_ARKADY, "(How sick)");
	c_speak(T_ARKADY, "I don't suppose I have much of a choice");
	c_wait(800);
	c_speak(T_PETER, "No, Arkady, you don't have a choice");
	c_speak(T_PETER, "Try to leave, and we'll just kill you on the spot");
	c_wait(1500);
	c_speak(T_MATTHEW, "Well then, take a seat - our esteemed guest and friend");
	c_wait(600);
	c_lookwalkto(C_ARKADY, 'K');
	c_lookat(C_ARKADY, 'I');
	c_teleportto(C_GERASIM, 'F');
	c_wait(400);
	c_walkto(C_GERASIM, 'H');
	c_lookat(C_ARKADY, 'H');
	c_wait(600);
	c_speak(T_ARKADY, "Come play with us, Gerasim");
	c_speak(T_GERASIM, "*He gestures as if to indicate the obviousness of such an action*");
	c_walkto(C_GERASIM, 'L');
	c_lookat(C_ARKADY, 'L');
	c_wait(500);
	c_lookat(C_ARKADY, 'I');
	c_wait(500);
	c_lookat(C_ARKADY, 'J');
	c_wait(500);
	c_lookat(C_ARKADY, 'I');
	c_speak(T_ARKADY, "So, what are we going to play?");
	c_speak(T_MATTHEW, "Do you know how to play Fool?");
	c_speak(T_ARKADY, "No, I can't say I do...");
	c_speak(T_MATTHEW, "That's alright, we'll show you how to play");
	c_wait(800);
	c_speak(T_MATTHEW, "Well then, shall we start?");
	
	// TODO: work on normal intro sequence.
}

static void
g_fastintro(void)
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
	c_speak(T_MATTHEW, "Well then, shall we start?");
}

static void
g_tutorial(void)
{
	c_pancamera((vec3){0.0f, 0.0f, 2.0f}, -90.0f, 0.0f);
	c_wait(1200);
	c_setdurakphase(D_START);
	c_wait(400);
	c_speak(T_MATTHEW, "This is a special deck, I've gone and removed some cards");
	c_speak(T_MATTHEW, "There are 36 cards - only 6-10s, face cards, and aces");
	c_speak(T_MATTHEW, "...");
	c_speak(T_MATTHEW, "The first step is to pick out a trump suit");
	c_speak(T_MATTHEW, "We'll pick out a card and select its suit");
	c_wait(300);
	c_setdurakphase(D_CHOOSETRUMP);
	c_wait(400);
	c_speak(T_MATTHEW, "Next we have to deal out 6 cards to each player");
	c_wait(300);
	c_setdurakphase(D_DEALCARDS);
	c_wait(400);
	c_speak(T_ARKADY, "So, how do we play?");
	c_speak(T_MATTHEW, "Fool is a game where you need to get rid of your held cards");
	c_speak(T_MATTHEW, "On my turn, I'll \"attack\" Gerasim, the player to my left, by laying out cards");
	c_speak(T_MATTHEW, "He must then cover them by playing a card of higher value...");
	c_speak(T_MATTHEW, "That's...");
	c_speak(T_MATTHEW, "Basically, the non-trump cards go from 6 to 10, then Jack, Queen, King, and finally Ace");
	c_speak(T_MATTHEW, "Only non-trump cards of equivalent suit can be used against each other");
	c_speak(T_MATTHEW, "And the trump cards can cover non-trump cards of any value");
	c_speak(T_MATTHEW, "...");
	c_speak(T_MATTHEW, "Uh...");
	c_speak(T_MATTHEW, "Right, then, when Gerasim covers my cards, I can give him more cards to cover");
	c_speak(T_MATTHEW, "But I can only play new cards with the same face value as previously played cards");
	c_speak(T_MATTHEW, "If he covers everything and I don't want to or can't attack him anymore, the attack ends, and he begins the next attack, this time on you");
	c_speak(T_MATTHEW, "If he can't cover my cards, he has to take all of the cards in play, and he skips his attack");
	c_speak(T_MATTHEW, "...");
	c_speak(T_MATTHEW, "Oh, and we all have to draw back up to six cards if we have less");
	c_speak(T_MATTHEW, "At least until the draw pile is exhausted");
	c_speak(T_MATTHEW, "That make sense?");
	c_speak(T_ARKADY, "I think I'll understand more once we actually start playing");
	c_speak(T_MATTHEW, "Oh...");
	c_speak(T_MATTHEW, "Well... No problem, of course, let's get started");
	c_speak(T_MATTHEW, "Just remember - defend, attack left, shed your cards");
	c_speak(T_MATTHEW, "Last player who still has cards loses");
	c_speak(T_ARKADY, "(I can feel my heart beating, I'm not ready)");
	c_speak(T_ARKADY, "I understand");
	c_wait(1000);
	c_speak(T_MATTHEW, "You all ready?");
	c_speak(T_GERASIM, "*Gerasim picks up his cards, he has clearly played before*");
	c_speak(T_ARKADY, "I think so");
	c_speak(T_PETER, "Well, I suppose I am, are you?");
	c_speak(T_MATTHEW, "I'm ready");
	c_wait(500);
	c_setdurakphase(D_ATTACK);
}
