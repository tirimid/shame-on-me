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
	"...R.....g..#...#" \
	"###b#########...#" \
	"...H..K.P.#.#.f.#" \
	"........Q.#.#####" \
	"....L.a.J.#......" \
	"..........#......" \
	"..NO..I.M.#......"

static void g_setupenv(void);

static usize g_maindoor, g_roomdoor;
static usize g_hallwaylight, g_entrylight, g_roomlight;

void
g_loop(bool fast)
{
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	c_reset();
	r_cut(R_FADEOUT);
	g_setupenv();
	if (fast)
	{
		d_state.round = 1;
		g_fastintroseq();
		g_roundendseq();
	}
	else
	{
		d_state.round = 0;
		g_introseq();
		g_tutorialseq();
	}
	
	for (;;)
	{
		begintick();
		
		// handle events.
		i_prepare();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			i_handle(&e);
			r_handle(&e);
			if (e.type == SDL_QUIT)
			{
				exit(0);
			}
		}
		
		// update.
		c_update();
		d_update();
		r_update();
		s_update();
		
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
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'f',
		(vec3){-0.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'h',
		(vec3){-0.5f, -0.3f, -0.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'h',
		(vec3){-0.5f, -0.3f, 0.5f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
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
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'I',
		(vec3){-1.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'I',
		(vec3){-2.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI / 2.0f, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
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
	// compute variable dialog.
	static char dialogtutorial[128] = {0};
	sprintf(dialogtutorial, "Progress dialog with %s", SDL_GetKeyName(o_dyn.ksel));
	
	c_speak(T_TUTORIAL, dialogtutorial);
	c_fade(R_FADEIN);
	c_lookwalkto(C_ARKADY, 'A');
	c_lookwalkto(C_ARKADY, 'B');
	c_lookat(C_ARKADY, 'C');
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_MUTEARKADY, "(I dread that this'll be what I thought)");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_MUTEARKADY, "(But I've already arrived)");
	c_speak(T_MUTEARKADY, "(It's too late to leave)");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_MUTEARKADY, "(There's no turning back now)");
	c_wait(300);
	c_walkto(C_ARKADY, 'C');
	c_lookat(C_ARKADY, 'D');
	c_wait(1500);
	c_playsfx(S_KNOCK);
	c_speak(T_MUTEARKADY, "...");
	c_wait(2500);
	c_teleportto(C_GERASIM, 'D');
	c_setlightintensity(g_entrylight, 1.5f);
	c_swapmodel(g_maindoor, R_DOOROPEN);
	c_wait(400);
	c_speak(T_MUTEGERASIM, "...");
	c_speak(T_MUTEGERASIM, "Gerasim stares at you regretfully");
	c_wait(1500);
	c_speak(T_ARKADY, "- It's good to see you again");
	c_speak(T_MUTEGERASIM, "...");
	c_walkto(C_GERASIM, 'E');
	c_walkto(C_ARKADY, 'D');
	c_swapmodel(g_maindoor, R_DOORCLOSED);
	c_setlightintensity(g_hallwaylight, 0.0f);
	c_lookat(C_ARKADY, 'E');
	c_speak(T_ARKADY, "- Are the others already here?");
	c_speak(T_MUTEGERASIM, "He nods with difficulty");
	c_speak(T_ARKADY, "- I see");
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
	c_speak(T_MATTHEW, "- Oh... You're already here, Arkady?");
	c_speak(T_MUTEMATTHEW, "Matthew's face looks about as friendly as it usually does");
	c_wait(300);
	c_speak(T_ARKADY, "- That's right");
	c_speak(T_MUTEARKADY, "(Act friendly, maybe I'm wrong, maybe it's not what I thought)");
	c_speak(T_PETER, "- Look, I'm sorry I didn't tell you why we needed you here");
	c_lookat(C_ARKADY, 'J');
	c_wait(100);
	c_speak(T_MUTEPETER, "Peter's eyes reflect the room's light in a particularly cynical way");
	c_speak(T_PETER, "- But that's kind of just the nature of the matter");
	c_speak(T_PETER, "- If I'd been honest, you wouldn't have come");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_ARKADY, "- Why did you call me over?");
	c_speak(T_PETER, "- You're aware of what's happening with the Earth's crust, right? The earthquakes?");
	c_speak(T_MUTEARKADY, "(I knew it)");
	c_speak(T_ARKADY, "- That's right");
	c_speak(T_PETER, "- Good. It's all over the news, you'd have to be real dim not to notice the apocalypse");
	c_wait(2000);
	c_speak(T_PETER, "- Anyway...");
	c_speak(T_PETER, "- With the country falling apart, the government can't sustain our population anymore");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_PETER, "- We have one day to decide who will die and who will live");
	c_speak(T_MUTEPETER, "...");
	c_speak(T_PETER, "- Or they will decide for us");
	c_wait(2500);
	c_speak(T_ARKADY, "- So you've brought me here like a lamb to the slaughterhouse?");
	c_speak(T_ARKADY, "- You're going to kill me so you can get off unscathed?");
	c_speak(T_PETER, "- Not quite, Arkady, we're going to give you a chance");
	c_speak(T_ARKADY, "- I don't understand");
	c_wait(500);
	c_speak(T_PETER, "- We're going to play some special 'high stakes' cards");
	c_speak(T_ARKADY, "- I don't suppose I have much of a choice");
	c_wait(800);
	c_speak(T_PETER, "- No, Arkady, you don't have a choice");
	c_wait(1500);
	c_speak(T_MATTHEW, "- Well then, take a seat");
	c_wait(600);
	c_lookwalkto(C_ARKADY, 'K');
	c_lookat(C_ARKADY, 'I');
	c_teleportto(C_GERASIM, 'F');
	c_wait(400);
	c_walkto(C_GERASIM, 'H');
	c_lookat(C_ARKADY, 'H');
	c_wait(600);
	c_speak(T_ARKADY, "- Come play with us, Gerasim");
	c_speak(T_MUTEGERASIM, "He gestures as if to indicate the obviousness of such an action");
	c_walkto(C_GERASIM, 'L');
	c_lookat(C_ARKADY, 'L');
	c_wait(500);
	c_lookat(C_ARKADY, 'I');
	c_wait(500);
	c_lookat(C_ARKADY, 'J');
	c_speak(T_PETER, "- Hey, Arkady");
	c_speak(T_ARKADY, "- Yes?");
	c_speak(T_PETER, "- Put this around your neck");
	c_speak(T_MUTEPETER, "He holds out a dodgily-built metal collar");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_ARKADY, "- Wait, what?");
	c_speak(T_PETER, "- We've gotta keep you honest if you lose");
	c_speak(T_PETER, "- Us three've already got ours on");
	c_lookat(C_ARKADY, 'L');
	c_wait(300);
	c_lookat(C_ARKADY, 'J');
	c_speak(T_MUTEARKADY, "(I glance under their coats, indeed they do)");
	c_speak(T_PETER, "- Put it on, Arkady.");
	c_speak(T_MUTEARKADY, "(As I clasp it around my neck, the weight of the situation dawns on me)");
	c_speak(T_MUTEARKADY, "...");
	c_speak(T_MUTEPETER, "Peter chuckles lightly under his breath");
	c_wait(1500);
	c_lookat(C_ARKADY, 'I');
	c_speak(T_ARKADY, "- What are we going to play?");
	c_speak(T_MATTHEW, "- Do you know how to play Fool?");
	c_speak(T_ARKADY, "- No, I can't say I do...");
	c_speak(T_MATTHEW, "- That's alright, we'll show you how to play a simplified variant");
	c_wait(800);
}

void
g_fastintroseq(void)
{
	c_fade(R_FADEIN);
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
}

void
g_tutorialseq(void)
{
	// compute variable dialog.
	char kselname[32], kskipname[32], krightname[32], kleftname[32];
	sprintf(kselname, "%s", SDL_GetKeyName(o_dyn.ksel));
	sprintf(kskipname, "%s", SDL_GetKeyName(o_dyn.kskip));
	sprintf(krightname, "%s", SDL_GetKeyName(o_dyn.kright));
	sprintf(kleftname, "%s", SDL_GetKeyName(o_dyn.kleft));
	
	static char keytutorial[256] = {0};
	sprintf(keytutorial, "You can move through your cards with %s and %s, select a card with %s, and do nothing with %s)", kleftname, krightname, kselname, kskipname);
	
	c_speak(T_MATTHEW, "- Well then, shall we start?");
	c_pancamera((vec3){0.0f, 0.0f, 2.0f}, -90.0f, 0.0f);
	c_wait(1200);
	c_setdurakphase(D_START);
	c_wait(400);
	c_speak(T_MATTHEW, "- This is a special deck, I've gone and removed some cards");
	c_speak(T_MATTHEW, "- There are 36 cards - only 6-10s, face cards, and aces");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "- The first step is to pick out a trump suit");
	c_speak(T_MATTHEW, "- We'll pick out a card and select its suit");
	c_speak(T_MATTHEW, "- Be sure to remember it");
	c_wait(300);
	c_setdurakphase(D_CHOOSETRUMP);
	c_wait(400);
	c_speak(T_MATTHEW, "- Next we have to deal out 6 cards to each player");
	c_wait(300);
	c_setdurakphase(D_DEALCARDS);
	c_wait(400);
	c_speak(T_ARKADY, "- So, how do we play?");
	c_speak(T_MATTHEW, "- Fool is a game where you need to get rid of your held cards");
	c_speak(T_MATTHEW, "- On my turn, I'll 'attack' Gerasim, the player to my left, by laying out a card of my choice");
	c_speak(T_MATTHEW, "- He must then 'cover' them by playing a card of higher value on top of it...");
	c_speak(T_MATTHEW, "- That's...");
	c_speak(T_MATTHEW, "- Basically, card values ascend from 6 to 10, then Jack, Queen, King, and finally Ace");
	c_speak(T_MATTHEW, "- And trump-suit cards are stronger than non-trump-suit cards");
	c_speak(T_MATTHEW, "- So, for example, if the trump suit is Clubs, then a 6 of Clubs would be better than an Ace of Diamonds...");
	c_speak(T_MATTHEW, "- But then a 7 of Clubs is still better than a 6...");
	c_speak(T_MATTHEW, "- Other than that, only cards of equivalent suit can be played against each other");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "- Uh...");
	c_speak(T_MATTHEW, "- Right, then, when Gerasim covers my cards, I can give him more cards to cover");
	c_speak(T_MATTHEW, "- But I can only play new cards with the same face value as previously played cards");
	c_speak(T_MATTHEW, "- If he covers everything and I don't want to or can't attack him anymore, the attack ends, and he begins the next attack, this time on you");
	c_speak(T_MATTHEW, "- If he can't cover my cards, he has to take all of the cards in play, and he skips his attack");
	c_speak(T_MUTEMATTHEW, "...");
	c_speak(T_MATTHEW, "- Oh, and we all have to draw back up to six cards if we have less");
	c_speak(T_MATTHEW, "- At least until the draw pile is exhausted");
	c_speak(T_MATTHEW, "- That make sense?");
	c_speak(T_ARKADY, "- I think I'll understand more once we actually start playing");
	c_speak(T_MATTHEW, "- Oh...");
	c_speak(T_MATTHEW, "- Well... No problem, of course...");
	c_speak(T_MATTHEW, "- Just remember - defend, attack left, shed your cards");
	c_speak(T_MATTHEW, "- Last player who still has cards loses");
	c_speak(T_MUTEARKADY, "(Even though it's just a practice round, I can feel my heart beating, I'm not ready)");
	c_speak(T_ARKADY, "- I understand");
	c_wait(1000);
	c_speak(T_MATTHEW, "- You all ready?");
	c_speak(T_MUTEGERASIM, "Gerasim picks up his cards, he has clearly played before");
	c_speak(T_ARKADY, "- I think so");
	c_speak(T_PETER, "- What difference does it make?");
	c_speak(T_MATTHEW, "- I suppose that's true");
	c_speak(T_MATTHEW, "- Let's just get started");
	c_speak(T_TUTORIAL, "The cards in your hand are automatically sorted and always go from weakest to strongest");
	c_speak(T_TUTORIAL, keytutorial);
	c_wait(500);
	c_setdurakphase(D_ATTACK);
}

void
g_roundendseq(void)
{
	if (d_state.round == 1)
	{
		// reset losses on tutorial round.
		for (usize i = 0; i < D_PLAYER_END; ++i)
		{
			d_state.losses[i] = 0;
		}
		
		// TODO: reenable after development.
#if 0
		c_wait(800);
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(1200);
		c_speak(T_MATTHEW, "- Since that was just a practice round, nothing happens");
		c_speak(T_MATTHEW, "- Are the rules of the game clearer now, Arkady?");
		c_speak(T_ARKADY, "- Yeah, I think so...");
		c_speak(T_MUTEMATTHEW, "...");
		c_speak(T_MATTHEW, "- That's great, probably");
		c_wait(600);
		c_shakecamera(O_QUAKESHAKE);
		c_wait(2000);
		c_speak(T_MATTHEW, "- I guess we should really get started...");
		c_speak(T_MATTHEW, "- This time, we're not just practicing");
		c_speak(T_MATTHEW, "- Let's say you need to lose twice to get out");
		c_speak(T_MATTHEW, "- Count your losses, you won't get a second chance");
		c_speak(T_ARKADY, "- Wait, will it reset between games?");
		c_wait(600);
		c_speak(T_MATTHEW, "- Hmm");
		c_wait(600);
		c_shakecamera(O_QUAKESHAKE);
		c_wait(1700);
		c_speak(T_MATTHEW, "- No it won't, we need to hurry up");
		c_wait(700);
#endif
	}
	else if (d_state.losses[D_ARKADY] == O_DEATHLOSSES)
	{
		g_arkadydeathseq();
	}
	else if (d_state.losses[D_PETER] == O_DEATHLOSSES)
	{
		g_peterdeathseq();
	}
	else if (d_state.losses[D_MATTHEW] == O_DEATHLOSSES)
	{
		g_matthewdeathseq();
	}
	else if (d_state.losses[D_GERASIM] == O_DEATHLOSSES)
	{
		g_gerasimdeathseq();
	}
	
	i32 ndead = 0;
	for (usize i = 0; i < D_PLAYER_END; ++i)
	{
		ndead += d_state.losses[i] >= O_DEATHLOSSES;
	}
	
	// arkady is the only one remaining.
	if (ndead == 3)
	{
		g_winseq();
		return;
	}
	
	c_pancamera((vec3){0.0f, 0.0f, 2.0f}, -90.0f, 0.0f);
	c_wait(1200);
	c_setdurakphase(D_START);
	c_wait(400);
	c_setdurakphase(D_CHOOSETRUMP);
	c_wait(400);
	c_setdurakphase(D_DEALCARDS);
	c_wait(400);
	c_setdurakphase(D_ATTACK);
}

void
g_arkadydeathseq(void)
{
	i32 whichdeath = 0;
	for (usize i = 0; i < D_PLAYER_END; ++i)
	{
		whichdeath += d_state.losses[i] >= O_DEATHLOSSES;
	}
	
	switch (whichdeath)
	{
	case 1:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(3000);
		c_speak(T_MUTEARKADY, "(I feel a sense of surreal forgetting wash over me)");
		c_speak(T_MUTEARKADY, "(The words I want to say evade me; and even if I could speak, I don't think I would)");
		c_speak(T_MUTEARKADY, "...");
		c_wait(1500);
		c_speak(T_MUTEMATTHEW, "Matthew looks at me, but I can't quite pick apart what he's thinking");
		c_speak(T_PETER, "- The rules are the rules, Arkady");
		c_lookat(C_ARKADY, 'J');
		c_speak(T_PETER, "- I think we'd all appreciate if you were to quietly step out");
		c_speak(T_PETER, "- I can't say I'd enjoy playing with my acquaintance's gray matter all over the wall");
		c_wait(700);
		c_speak(T_PETER, "- Get moving!");
		c_speak(T_MUTEARKADY, "(I feel the collar constricting me)");
		c_lookat(C_ARKADY, 'L');
		c_speak(T_MUTEGERASIM, "Gerasim's eyes mirror the reflection of a dead man");
		c_lookat(C_ARKADY, 'I');
		c_speak(T_MUTEARKADY, "(Please, do something)");
		c_speak(T_PETER, "- Son of a...");
		c_shakecamera(O_PLAYERDEATHSHAKE);
		c_playsfx(S_EXPLODE);
		c_globalshade((vec3)O_DEATHGLOBALSHADE);
		c_wait(100);
		c_quit();
		break;
	case 2:
		if (d_state.losses[D_PETER] == D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1400);
			c_lookat(C_ARKADY, 'L');
			c_wait(800);
			c_lookat(C_ARKADY, 'I');
			c_wait(1600);
			c_speak(T_MATTHEW, "- I'm sorry Arkady");
			c_speak(T_MUTEARKADY, "(I try to raise my voice but I can't)");
			c_speak(T_MATTHEW, "- I thought this would end differently...");
			c_speak(T_MATTHEW, "- Or...");
			c_speak(T_MATTHEW, "- No, I don't know how I thought this would end");
			c_wait(1800);
			c_lookat(C_ARKADY, 'L');
			c_speak(T_MUTEGERASIM, "Gerasim knows he can't do anything, he stares void of any emotion");
			c_wait(300);
			c_lookat(C_ARKADY, 'I');
			c_speak(T_ARKADY, "- I'll leave");
			c_speak(T_MUTEARKADY, "(It's pressing into my neck, It hurts so much)");
			c_speak(T_MUTEARKADY, "(I need to hurry)");
			c_lookwalkto(C_ARKADY, 'H');
			c_lookat(C_ARKADY, 'F');
			c_wait(400);
			c_setlightintensity(g_entrylight, 1.5f);
			c_swapmodel(g_roomdoor, R_DOOROPEN);
			c_wait(300);
			c_pancamera((vec3){0}, -20.0f, 0.0f);
			c_wait(800);
			c_speak(T_MUTEARKADY, "(Oh my god)");
			// TODO: finish this scene with corpse texture.
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		else if (d_state.losses[D_MATTHEW] == D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1700);
			c_speak(T_PETER, "- You know what to do, scram");
			c_lookat(C_ARKADY, 'J');
			c_speak(T_MUTEARKADY, "(I feel a boiling rage)");
			c_speak(T_ARKADY, "- How could you just move on so fucking easily?");
			c_speak(T_PETER, "- I'm not sure what you mean, Arkady");
			c_speak(T_ARKADY, "- First Matthew, now me, you just sit idly by?");
			c_speak(T_PETER, "- I-");
			c_speak(T_ARKADY, "- No, not even idly, you don't even give us the courtesy to die in dignity");
			c_speak(T_ARKADY, "- I never fucking liked you, I never cared for anything you ever did");
			c_speak(T_ARKADY, "- You were always a cancer on our group");
			c_speak(T_MUTEARKADY, "(The collar is digging into my flesh)");
			c_speak(T_MUTEARKADY, "(I know it'll go off at any moment)");
			c_speak(T_ARKADY, "- I'll take you with me, you worthless fucking parasite");
			c_lookwalkto(C_ARKADY, 'P');
			c_lookwalkto(C_ARKADY, 'Q');
			c_wait(300);
			c_speak(T_PETER, "- Get the hell off me!");
			c_lookat(C_ARKADY, 'L');
			c_speak(T_ARKADY, "- Gerasim, run!");
			c_lookat(C_ARKADY, 'J');
			c_wait(400);
			c_speak(T_PETER, "- Wait-");
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		else // gerasim.
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(2700);
			c_speak(T_ARKADY, "- Matthew, I'm scared");
			c_wait(1800);
			c_speak(T_MATTHEW, "- Arkady, we all are");
			c_speak(T_ARKADY, "- Gerasim was so accepting of it, but how? I don't want to die...");
			c_speak(T_MATTHEW, "- Well, he was mute, we'll never get the answer");
			c_speak(T_MATTHEW, "- He was never the particularly 'emotional' type");
			c_speak(T_MATTHEW, "- With how he reacted to his loss, I can't help but think that was a front");
			c_speak(T_ARKADY, "- Why? He knows... knew... we were his friends");
			c_speak(T_MATTHEW, "- Arkady, I don't know");
			c_speak(T_MATTHEW, "- Maybe he was confused in the moment, or he knew that nothing could be done");
			c_speak(T_PETER, "- Will you precious snowflakes hurry it up already?");
			c_lookat(C_ARKADY, 'J');
			c_speak(T_PETER, "- You know damn well yourself, you can't do anything about this");
			c_wait(1600);
			c_lookat(C_ARKADY, 'I');
			c_speak(T_ARKADY, "- I'm scared");
			c_speak(T_MUTEMATTHEW, "...");
			c_speak(T_ARKADY, "- I don't want to die, I don't");
			c_speak(T_MUTEARKADY, "(Well, I say this, but I'm not sure if it's true - I'm too muddled in the head right now to form judgements on what I really do or don't want)");
			c_speak(T_MUTEARKADY, "(Hmm, maybe this is how he felt...)");
			c_speak(T_MUTEARKADY, "(What am I even thinking about? Have I forgotten what's happening?)");
			c_speak(T_MATTHEW, "- Arkady, your collar, it's-");
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		break;
	case 3:
		if (d_state.losses[D_PETER] != D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(2300);
			c_speak(T_MUTEPETER, "He grins with characteristic disregard for anything that affects others");
			c_speak(T_PETER, "- End of the road, Arkady");
			c_speak(T_MUTEARKADY, "...");
			c_speak(T_PETER, "- Look, if there's one thing I should say...");
			c_speak(T_PETER, "- To be fully honest, it wasn't completely your fault that you lost");
			c_speak(T_MUTEPETER, "Peter's countenance reproduces the gaze of somebody who thinks he's being very smart");
			c_speak(T_PETER, "- I don't just mean the inherent randomness in the game, no");
			c_speak(T_PETER, "- Let's just say I'm not surprised that I won");
			c_speak(T_PETER, "- Heh");
			c_wait(3000);
			c_speak(T_ARKADY, "- Peter");
			c_speak(T_ARKADY, "- If there is a God, and if there is a Hell");
			c_speak(T_ARKADY, "- A-an honest god, and if he exists, he will... He will-");
			c_speak(T_PETER, "- Ahem...");
			c_wait(400);
			c_walkto(C_PETER, 'O');
			c_walkto(C_PETER, 'H');
			c_lookat(C_ARKADY, 'H');
			c_speak(T_PETER, "- No need to finish your sentence, Arkady, you're predictable to a tee");
			c_speak(T_PETER, "- Have the room to yourself - to die");
			c_speak(T_PETER, "- I'll see you in hell");
			c_walkto(C_PETER, 'F');
			c_wait(2300);
			c_lookat(C_ARKADY, 'I');
			c_pancamera((vec3){0.0f, -0.3f, 0.4f}, -20.0f, 0.0f);
			c_wait(1800);
			c_speak(T_MUTEARKADY, "(The collar's pushing in on me)");
			c_speak(T_ARKADY, "- I guess I will");
			c_wait(1600);
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		else if (d_state.losses[D_MATTHEW] != D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(900);
			c_speak(T_MATTHEW, "- I didn't want to win");
			c_speak(T_MUTEMATTHEW, "He is wide-eyed");
			c_speak(T_MATTHEW, "- I've been playing all my weakest cards, I thought you would win");
			c_speak(T_MATTHEW, "- I couldn't in good conscience let you die...");
			c_wait(1000);
			c_speak(T_ARKADY, "- Well, you did");
			c_wait(1000);
			c_speak(T_MATTHEW, "- Arkady, forgive me, please");
			c_speak(T_ARKADY, "- I never held anything against you");
			c_speak(T_ARKADY, "- But I don't want to die, Matthew");
			c_speak(T_ARKADY, "- I think...");
			c_speak(T_MUTEARKADY, "(I feel like I should be upset, but really I'm kind of indifferent)");
			c_speak(T_MUTEARKADY, "(Is this what it's like to be desensitized to everything?)");
			c_speak(T_ARKADY, "- No, no, I just feel kind of empty");
			c_wait(1300);
			c_speak(T_MATTHEW, "- I'm sorry");
			c_speak(T_ARKADY, "- No, there's nothing to forgive you for");
			c_speak(T_ARKADY, "- I'll step outside");
			c_wait(600);
			c_speak(T_MUTEMATTHEW, "After processing your words, he nods");
			c_lookwalkto(C_ARKADY, 'H');
			c_lookat(C_ARKADY, 'F');
			c_wait(600);
			c_speak(T_MATTHEW, "- Wait!");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MATTHEW, "- Arkady, I'll miss you");
			c_wait(500);
			c_lookat(C_ARKADY, 'F');
			// TODO: finish scene with corpse textures.
			c_setlightintensity(g_entrylight, 1.5f);
			c_swapmodel(g_roomdoor, R_DOOROPEN);
			c_wait(300);
			c_pancamera((vec3){0}, -20.0f, 0.0f);
			c_wait(500);
			c_speak(T_MUTEARKADY, "(I see)");
			c_pancamera((vec3){0}, -20.0f, 0.0f);
			c_wait(800);
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_lookwalkto(C_ARKADY, 'R');
			c_lookat(C_ARKADY, 'b');
			c_speak(T_MUTEARKADY, "(Death is coming)");
			c_speak(T_MUTEARKADY, "(I hope you don't see this, Matthew)");
			c_setlightintensity(g_roomlight, 0.0f);
			c_swapmodel(g_roomdoor, R_DOORCLOSED);
			c_wait(900);
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		else // gerasim.
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1300);
			c_speak(T_MUTEARKADY, "(I'm in shock)");
			c_speak(T_MUTEARKADY, "(Peter died, Matthew died, and I'm about to die)");
			c_speak(T_MUTEARKADY, "(I couldn't believe it if it didn't feel so real)");
			c_speak(T_MUTEARKADY, "(Please tell me this is all a bad dream)");
			c_speak(T_MUTEARKADY, "(The earthquakes, the game, it shouldn't be real, it doesn't make sense)");
			c_speak(T_MUTEGERASIM, "...");
			c_speak(T_MUTEGERASIM, "Gerasim gestures like he'll get up");
			c_speak(T_MUTEARKADY, "(No, wait, you don't need to, I'll leave)");
			c_speak(T_MUTEARKADY, "(Nothing's coming out)");
			c_wait(300);
			c_walkto(C_GERASIM, 'O');
			c_walkto(C_GERASIM, 'H');
			c_lookat(C_ARKADY, 'H');
			c_speak(T_MUTEARKADY, "(Wait)");
			c_walkto(C_GERASIM, 'F');
			c_wait(800);
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MUTEARKADY, "(Oh god)");
			c_speak(T_MUTEARKADY, "(Everything's closing in on me)");
			c_speak(T_MUTEARKADY, "(The collar-)");
			c_shakecamera(O_PLAYERDEATHSHAKE);
			c_playsfx(S_EXPLODE);
			c_globalshade((vec3)O_DEATHGLOBALSHADE);
			c_wait(100);
			c_quit();
		}
		break;
	}
}

void
g_peterdeathseq(void)
{
	i32 whichdeath = 0;
	for (usize i = 0; i < D_PLAYER_END; ++i)
	{
		whichdeath += d_state.losses[i] >= O_DEATHLOSSES;
	}
	
	switch (whichdeath)
	{
	case 1:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(900);
		c_lookat(C_ARKADY, 'J');
		c_wait(1200);
		c_speak(T_PETER, "- No, I honestly don't know what to say");
		c_speak(T_PETER, "- I knew the possibility, but... the first one out?");
		c_speak(T_MUTEPETER, "Peter can barely countenance your gaze");
		c_wait(800);
		c_speak(T_PETER, "- No, no, I'll step out");
		c_speak(T_PETER, "- I don't want you to see this");
		c_wait(300);
		c_walkto(C_PETER, 'M');
		c_walkto(C_MATTHEW, 'N');
		c_walkto(C_PETER, 'O');
		c_lookat(C_ARKADY, 'O');
		c_speak(T_MUTEPETER, "...");
		c_walkto(C_PETER, 'H');
		c_lookat(C_ARKADY, 'H');
		c_walkto(C_MATTHEW, 'I');
		c_speak(T_PETER, "- I didn't think it would go this way");
		c_walkto(C_PETER, 'F');
		c_wait(1300);
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(600);
		c_speak(T_MUTEARKADY, "(I feel nauseous)");
		c_lookat(C_ARKADY, 'I');
		c_wait(600);
		c_speak(T_MATTHEW, "- We knew it would happen");
		c_speak(T_MATTHEW, "- Be glad it wasn't you");
		c_speak(T_MATTHEW, "- It may not stay that way for long");
		c_wait(900);
		c_speak(T_MUTEARKADY, "(I probably look so pathetic right now)");
		break;
	case 2:
		if (d_state.losses[D_MATTHEW] == D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1700);
			c_speak(T_PETER, "- I'm not the first one out");
			c_speak(T_PETER, "- I mean...");
			c_speak(T_PETER, "- That's more than our friend could say");
			c_speak(T_MUTEPETER, "...");
			c_speak(T_PETER, "- Heh");
			c_speak(T_MUTEPETER, "His forced casual attitude is spectacularly failing to hide his true feelings");
			c_wait(1200);
			c_speak(T_MUTEPETER, "He covers his face subtly with his hat");
			c_speak(T_PETER, "- I'll leave about now");
			c_speak(T_PETER, "- I feel the bomb collar taking its hold");
			c_wait(600);
			c_walkto(C_PETER, 'O');
			c_lookat(C_ARKADY, 'O');
			c_walkto(C_PETER, 'H');
			c_lookat(C_ARKADY, 'H');
			c_wait(700);
			c_speak(T_MUTEPETER, "Right when he's about to step out, Peter lifts his hat and glares at you and Gerasim with rage");
			c_speak(T_PETER, "- You know what?");
			c_speak(T_PETER, "- Fuck you, fuck you both, rot in hell");
			c_wait(600);
			c_walkto(C_PETER, 'F');
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(2400);
			c_speak(T_MUTEARKADY, "(I'm... so profoundly... confused)");
			c_lookat(C_ARKADY, 'L');
			c_wait(700);
			c_speak(T_MUTEGERASIM, "Gerasim's face paints the pang of betrayal");
			c_speak(T_ARKADY, "- What just happened?");
			c_speak(T_MUTEGERASIM, "He grabs hold of himself and shuffles over where Peter sat");
			c_walkto(C_GERASIM, 'O');
			c_walkto(C_GERASIM, 'I');
			c_lookat(C_ARKADY, 'I');
			c_wait(400);
			c_speak(T_MUTEARKADY, "(I guess he wants to deal...)");
			d_state.handpos[D_GERASIM] = D_TOP;
		}
		else // gerasim.
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1900);
			c_lookat(C_ARKADY, 'J');
			c_speak(T_PETER, "- What a waste");
			c_speak(T_PETER, "- I can't believe this is happening");
			c_speak(T_PETER, "- It doesn't even make any sense, how?");
			c_speak(T_PETER, "- It doesn't make some sense, none of it, it doesn't make any sense, it doesn't make any of it");
			c_speak(T_PETER, "- What does that even mean? Can you tell me, Arkady? What's the meaning of this?");
			c_speak(T_MUTEARKADY, "...");
			c_speak(T_MATTHEW, "- He's incoherent");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MATTHEW, "- It's a sad sight, so deep in his own fear, I doubt he even understands what we're saying");
			c_speak(T_PETER, "- No...");
			c_lookat(C_ARKADY, 'J');
			c_speak(T_PETER, "- No, I need to be alone");
			c_speak(T_ARKADY, "- Peter, what's happening?");
			c_speak(T_MUTEARKADY, "(He's clearly not in sane mind)");
			c_wait(4000);
			c_walkto(C_PETER, 'M');
			c_lookat(C_ARKADY, 'M');
			c_speak(T_PETER, "- Move!");
			c_speak(T_MATTHEW, "- Uh... I, alright...");
			c_lookat(C_ARKADY, 'I');
			c_walkto(C_MATTHEW, 'N');
			c_walkto(C_PETER, 'O');
			c_walkto(C_PETER, 'H');
			c_walkto(C_MATTHEW, 'I');
			c_lookat(C_ARKADY, 'H');
			c_walkto(C_PETER, 'F');
			c_wait(400);
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(2400);
			c_lookat(C_ARKADY, 'I');
			c_wait(1600);
			c_speak(T_ARKADY, "- What was that?");
			c_speak(T_ARKADY, "- It's like his entire world just inverted");
			c_speak(T_MUTEMATTHEW, "Matthew looks confused yet indifferent");
			c_speak(T_MATTHEW, "- I don't know, Arkady");
			c_speak(T_MATTHEW, "- The human mind is capable of utterly bizzare things");
			c_speak(T_MUTEMATTHEW, "...");
			c_speak(T_MATTHEW, "- The realization that you're taking your final breaths wouldn't weigh lightly on anybody");
			c_speak(T_MUTEARKADY, "...");
			c_speak(T_ARKADY, "- I suppose so");
			c_wait(800);
			c_speak(T_MUTEARKADY, "(This is strange)");
			c_speak(T_MUTEARKADY, "(My friend just died but I feel almost nothing)");
			c_speak(T_MUTEARKADY, "(What's wrong with me?)");
			c_speak(T_MATTHEW, "- Arkady?");
			c_speak(T_ARKADY, "- Oh?");
			c_speak(T_MATTHEW, "- It's just the two of us now");
			c_speak(T_MATTHEW, "- Should we continue?");
			c_wait(500);
			c_speak(T_MUTEARKADY, "I nod, it's the beginning of the end");
		}
		break;
	case 3:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(1500);
		c_speak(T_MUTEPETER, "He looks subtly to the side and makes a confused expression");
		c_speak(T_PETER, "- You know...");
		c_speak(T_PETER, "- I really didn't expect this");
		c_speak(T_PETER, "- I honestly thought I'd win");
		c_speak(T_PETER, "- I even took some light 'precautions' to guarantee that");
		c_speak(T_MUTEPETER, "...");
		c_speak(T_PETER, "- Well I've failed");
		c_speak(T_PETER, "- I've failed and I'm gonna die now");
		c_speak(T_MUTEPETER, "Peter takes exaggerated breaths to mask his worry");
		c_speak(T_ARKADY, "- You know, Peter...");
		c_speak(T_ARKADY, "- I'm not even surprised you tried to cheat us on this");
		c_speak(T_ARKADY, "- Well, 'tried'; you got two of us");
		c_speak(T_ARKADY, "- I'll be honest with you, Peter, I never liked you. I was suspicious of you from the very first moment you started associating with us");
		c_speak(T_ARKADY, "- I tolerated you because I thought Gerasim and Matthew saw something in you that I didn't");
		c_speak(T_ARKADY, "- I thought you could change, Peter");
		c_speak(T_MUTEPETER, "He doesn't even try to retaliate");
		c_speak(T_ARKADY, "- From the moment I met you, to the moment you took yourself down with the rest of us, you've cheated and played us");
		c_wait(800);
		c_speak(T_PETER, "- I'm leaving");
		c_speak(T_ARKADY, "- Good");
		c_walkto(C_PETER, 'O');
		c_walkto(C_PETER, 'H');
		c_lookat(C_ARKADY, 'H');
		c_speak(T_MUTEPETER, "Peter shakes his head before opening the door");
		c_wait(400);
		c_speak(T_ARKADY, "- Good riddance");
		c_walkto(C_PETER, 'F');
		c_wait(200);
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(4000);
		c_speak(T_MUTEARKADY, "(What now?)");
		c_lookat(C_ARKADY, 'I');
		break;
	}
}

void
g_matthewdeathseq(void)
{
	i32 whichdeath = 0;
	for (usize i = 0; i < D_PLAYER_END; ++i)
	{
		whichdeath += d_state.losses[i] >= O_DEATHLOSSES;
	}
	
	switch (whichdeath)
	{
	case 1:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(900);
		c_speak(T_MUTEMATTHEW, "As the last attack finishes, Matthew's face pales");
		c_speak(T_MUTEMATTHEW, "He looks around at the others, as if expecting them to help");
		c_speak(T_MATTHEW, "- Wait, seriously?");
		c_speak(T_MATTHEW, "- Seriously?");
		c_speak(T_MUTEPETER, "Peter projects a smug faux-sympathy");
		c_lookat(C_ARKADY, 'J');
		c_speak(T_PETER, "- You knew what you were getting into");
		c_lookat(C_ARKADY, 'I');
		c_speak(T_MUTEARKADY, "(It's the unmistakable expression a person makes when they know they're about to die)");
		c_speak(T_PETER, "- Fuck outta here!");
		c_lookat(C_ARKADY, 'J');
		c_speak(T_PETER, "- You better not leave your goddamn brain juices on the table when you die!");
		c_speak(T_MATTHEW, "- I can't take this");
		c_lookat(C_ARKADY, 'I');
		c_walkto(C_MATTHEW, 'O');
		c_lookat(C_ARKADY, 'I');
		c_walkto(C_MATTHEW, 'H');
		c_lookat(C_ARKADY, 'H');
		c_speak(T_ARKADY, "- I'm sorr-");
		c_walkto(C_MATTHEW, 'F');
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(1800);
		c_speak(T_PETER, "- I'll deal the cards this time");
		c_lookat(C_ARKADY, 'J');
		c_walkto(C_PETER, 'M');
		c_walkto(C_PETER, 'I');
		c_lookat(C_ARKADY, 'I');
		c_speak(T_MUTEARKADY, "(Like nothing happened...)");
		d_state.handpos[D_PETER] = D_TOP;
		break;
	case 2:
		if (d_state.losses[D_PETER] == D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1300);
			c_speak(T_MATTHEW, "- Arkady, Gerasim");
			c_lookat(C_ARKADY, 'L');
			c_speak(T_MUTEGERASIM, "...");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_ARKADY, "- Matthew");
			c_speak(T_MUTEARKADY, "(My heart's sinking looking at him)");
			c_speak(T_ARKADY, "- Matthew, I didn't want this to happen");
			c_wait(800);
			c_speak(T_ARKADY, "- I didn't want any of this");
			c_speak(T_MATTHEW, "- I know you didn't, Arkady, I don't really even blame you for it");
			c_speak(T_MATTHEW, "- If it's anybody's fault here, it's Peter's... He had the idea, he got us all together");
			c_speak(T_MATTHEW, "- Kind of ironic, really, that he was the first one out");
			c_wait(400);
			c_speak(T_ARKADY, "- What do you think happens after death?");
			c_speak(T_MATTHEW, "- I don't know");
			c_wait(1500);
			c_walkto(C_MATTHEW, 'O');
			c_walkto(C_MATTHEW, 'H');
			c_lookat(C_ARKADY, 'H');
			c_speak(T_MATTHEW, "- Gerasim, you can deal");
			c_walkto(C_MATTHEW, 'F');
			c_wait(700);
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(1500);
			c_lookat(C_ARKADY, 'L');
			c_walkto(C_GERASIM, 'O');
			c_walkto(C_GERASIM, 'I');
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MUTEARKADY, "(I can't lose, but at the same time, I can't find myself wanting to win)");
			d_state.handpos[D_GERASIM] = D_TOP;
		}
		else // gerasim.
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(800);
			c_speak(T_MATTHEW, "- Peter, it's not the fucking time!");
			c_speak(T_PETER, "- Just a little good-natured teasing, eh?");
			c_lookat(C_ARKADY, 'J');
			c_speak(T_MATTHEW, "- Stop fucking kicking me under the table!");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MUTEARKADY, "(He's going to die...)");
			c_speak(T_MATTHEW, "- You can't let me do anything in peace!");
			c_speak(T_MATTHEW, "- Not even die! I can't even die in peace with you around!");
			c_speak(T_MATTHEW, "- Why did we ever keep you around?");
			c_walkto(C_MATTHEW, 'O');
			c_walkto(C_MATTHEW, 'H');
			c_lookat(C_ARKADY, 'H');
			c_walkto(C_MATTHEW, 'F');
			c_wait(600);
			c_speak(T_PETER, "- Check this out");
			c_lookat(C_ARKADY, 'J');
			c_speak(T_PETER, "- 3...");
			c_speak(T_PETER, "- 2...");
			c_speak(T_PETER, "- 1...");
			c_wait(400);
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(300);
			c_speak(T_PETER, "- Heh");
			c_wait(700);
			c_speak(T_ARKADY, "- How can you live with yourself, Peter?");
			c_speak(T_ARKADY, "- What did Matthew see in you?");
			c_speak(T_ARKADY, "- What did Gerasim see in you?");
			c_wait(300);
			c_speak(T_MUTEPETER, "He's slightly cracking under the pressure");
			c_speak(T_PETER, "- I'm dealing");
			c_walkto(C_PETER, 'M');
			c_walkto(C_PETER, 'I');
			c_lookat(C_ARKADY, 'I');
			c_speak(T_MUTEARKADY, "(How disgusting, I don't even feel afraid anymore, I'm angry)");
			d_state.handpos[D_PETER] = D_TOP;
		}
		break;
	case 3:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(2600);
		c_speak(T_ARKADY, "- Matthew?");
		c_speak(T_MUTEMATTHEW, "Matthew is deep in thought");
		c_speak(T_MATTHEW, "- Arkady...");
		c_speak(T_MATTHEW, "- You know how people sometimes reveal secrets when they're going to die?");
		c_speak(T_ARKADY, "- Do they?");
		c_speak(T_MATTHEW, "- Er, I don't really know");
		c_speak(T_MATTHEW, "- I just really wanted to tell you some things");
		c_speak(T_MUTEMATTHEW, "He shifts uncomfortably");
		c_speak(T_MATTHEW, "- But I just can't");
		c_speak(T_MATTHEW, "- Isn't that weird? I have no reason to keep anything hidden anymore, but I can't bring myself to speak");
		c_speak(T_MATTHEW, "- And there's a lot I want to tell you");
		c_speak(T_MATTHEW, "- In just a few seconds, I'll never get to say anything ever again");
		c_speak(T_MATTHEW, "- And yet in this critical moment I'm self-silencing");
		c_wait(700);
		c_speak(T_ARKADY, "- What did you want to tell me?");
		c_wait(800);
		c_speak(T_MATTHEW, "- Too much, Arkady");
		c_speak(T_MATTHEW, "- I can't believe you'll never hear it");
		c_speak(T_MUTEARKADY, "(Is he about to cry?)");
		c_wait(500);
		c_speak(T_MATTHEW, "- No point in wasting any more time, I can feel death approaching");
		c_speak(T_MATTHEW, "- Arkady, I'm sorry, I hope you live a long and happy life");
		c_walkto(C_MATTHEW, 'O');
		c_walkto(C_MATTHEW, 'H');
		c_lookat(C_ARKADY, 'H');
		c_speak(T_MUTEMATTHEW, "He looks like he wants to say something, then waves his hand at the prospect");
		c_walkto(C_MATTHEW, 'F');
		c_wait(300);
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(2000);
		c_speak(T_MUTEARKADY, "(Matthew...)");
		c_lookat(C_ARKADY, 'I');
		break;
	}
}

void
g_gerasimdeathseq(void)
{
	i32 whichdeath = 0;
	for (usize i = 0; i < D_PLAYER_END; ++i)
	{
		whichdeath += d_state.losses[i] >= O_DEATHLOSSES;
	}
	
	switch (whichdeath)
	{
	case 1:
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(900);
		c_lookat(C_ARKADY, 'L');
		c_wait(2000);
		c_speak(T_MUTEGERASIM, "Gerasim looks right at you");
		c_speak(T_MUTEARKADY, "(His expression is unreadable)");
		c_wait(800);
		c_walkto(C_GERASIM, 'H');
		c_lookat(C_ARKADY, 'H');
		c_wait(700);
		c_speak(T_MUTEGERASIM, "He gestures lightly and waves, as if bidding farewell");
		c_wait(900);
		c_walkto(C_GERASIM, 'F');
		c_wait(600);
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(1300);
		c_speak(T_MATTHEW, "- The truth is, I know he isn't that upset about it");
		c_lookat(C_ARKADY, 'I');
		c_speak(T_MATTHEW, "- Well, wasn't...");
		c_speak(T_MUTEMATTHEW, "...");
		c_wait(500);
		c_speak(T_ARKADY, "- Why'd he leave?");
		c_speak(T_MATTHEW, "- Being considerate, I think");
		c_speak(T_MATTHEW, "- I mean... We'll never know anymore");
		c_wait(800);
		c_speak(T_MUTEARKADY, "(I'm so scared, I just wish this were all over)");
		break;
	case 2:
		if (d_state.losses[D_PETER] == D_DEAD)
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(900);
			c_lookat(C_ARKADY, 'L');
			c_wait(1200);
			c_speak(T_ARKADY, "- Hey, Gerasim, I know this is a bad time, but I have a question");
			c_speak(T_ARKADY, "- I just figured that, well, I'll never be able to ask again");
			c_speak(T_MUTEGERASIM, "Gerasim nods, awaiting the question");
			c_speak(T_ARKADY, "- Why were you friends with Peter?");
			c_speak(T_ARKADY, "- Why did you introduce him to us?");
			c_speak(T_ARKADY, "- I hate to talk behind the back of a dead man, but...");
			c_wait(400);
			c_lookat(C_ARKADY, 'I');
			c_wait(300);
			c_lookat(C_ARKADY, 'L');
			c_wait(300);
			c_speak(T_ARKADY, "- Well, I'm sure you know what I mean");
			c_wait(1200);
			c_speak(T_MUTEGERASIM, "He can't even respond, shrugging");
			c_speak(T_ARKADY, "- I'm sorry for asking");
			c_wait(2300);
			c_walkto(C_GERASIM, 'H');
			c_lookat(C_ARKADY, 'H');
			c_wait(700);
			c_walkto(C_GERASIM, 'F');
			c_wait(300);
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(1600);
			c_lookat(C_ARKADY, 'I');
			c_speak(T_ARKADY, "- I can't believe I did that");
			c_speak(T_ARKADY, "- What a horrible thing to do, interrogate somebody about some bullshit drama right before their head is blown off");
			c_speak(T_ARKADY, "- I shouldn't have done, that, should I?");
			c_wait(600);
			c_speak(T_MATTHEW, "- There's nothing that can be done anymore, Arkady");
			c_wait(800);
			c_speak(T_ARKADY, "- You're right");
			c_speak(T_MUTEARKADY, "(How horrible, and right before the last game too)");
		}
		else // matthew.
		{
			c_pancamera((vec3){0}, 0.0f, 0.0f);
			c_wait(1100);
			c_lookat(C_ARKADY, 'L');
			c_wait(500);
			c_speak(T_MUTEGERASIM, "Gerasim accepts his fate as the last card is laid down");
			c_speak(T_MUTEGERASIM, "His eyes hide some indiscernible emotion");
			c_speak(T_PETER, "- Are you just going to keep looking at each other?");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_PETER, "- I can hear the collar beeping all the way from here!");
			c_speak(T_MUTEARKADY, "...");
			c_speak(T_ARKADY, "- Peter, the collars don't beep");
			c_speak(T_PETER, "- I know, asshole!");
			c_speak(T_PETER, "- I'm the one who made them!");
			c_speak(T_ARKADY, "- What are you saying they beep for, then?");
			c_speak(T_PETER, "- It's a nicer way to get 'im outta here");
			c_lookat(C_ARKADY, 'L');
			c_speak(T_MUTEGERASIM, "Gerasim shows no animosity, he just wants the affair to be over as soon as possible");
			c_speak(T_MUTEGERASIM, "He actions, standing up");
			c_wait(400);
			c_walkto(C_GERASIM, 'H');
			c_lookat(C_ARKADY, 'H');
			c_speak(T_MUTEARKADY, "(I find myself wanting to say something)");
			c_wait(500);
			c_speak(T_ARKADY, "- Hey-");
			c_walkto(C_GERASIM, 'F');
			c_wait(200);
			c_shakecamera(O_DEATHSHAKE);
			c_playsfx(S_EXPLODEMUFFLED);
			c_wait(900);
			c_speak(T_PETER, "- Time for the last game");
			c_lookat(C_ARKADY, 'I');
			c_speak(T_PETER, "- Ready?");
			c_speak(T_ARKADY, "- I-");
			c_speak(T_PETER, "- Don't give a shit, here's your cards");
			c_speak(T_MUTEARKADY, "(Seriously, again?)");
		}
		break;
	case 3:
		c_teleportto(C_GERASIM, 'F');
		c_pancamera((vec3){0}, 0.0f, 0.0f);
		c_wait(600);
		c_speak(T_MUTEARKADY, "(What?)");
		c_wait(1800);
		c_lookat(C_ARKADY, 'J');
		c_wait(400);
		c_lookat(C_ARKADY, 'L');
		c_wait(400);
		c_lookat(C_ARKADY, 'H');
		c_wait(250);
		c_speak(T_ARKADY, "- Gerasim?");
		c_speak(T_ARKADY, "- Gerasim, are you there?");
		c_wait(1400);
		c_shakecamera(O_DEATHSHAKE);
		c_playsfx(S_EXPLODEMUFFLED);
		c_wait(2500);
		c_speak(T_MUTEARKADY, "(Even he's gone now)");
		c_wait(600);
		c_lookat(C_ARKADY, 'I');
		break;
	}
}

void
g_winseq(void)
{
	// TODO: implement win sequence.
}
