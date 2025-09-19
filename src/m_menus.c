// SPDX-License-Identifier: GPL-3.0-or-later

static void m_textscroll(char const *lines[], usize nlines);

static f32 m_cycle;

void
m_main(void)
{
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	c_reset();
	r_cut(R_FADEIN);
	
	// set map.
	c_map = (c_map_t)
	{
		.data =
			"....."
			"....."
			".c.ab"
			"....."
			".....",
		.w = 5,
		.h = 5
	};
	
	// place lights.
	c_putlightat('c', (vec3){0.0f, 0.0f, 0.0f}, 0.4f);
	
	// place props.
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'b',
		(vec3){0.5f, -0.3f, -0.5f},
		(vec3){0.0f, GLM_PI, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MWINDOW,
		R_TWINDOW,
		'b',
		(vec3){0.5f, -0.3f, 0.5f},
		(vec3){0.0f, GLM_PI, 0.0f},
		(vec3){0.3f, 0.5f, 0.8f}
	);
	
	c_putpropat(
		R_MTABLE,
		R_TTABLE,
		'a',
		(vec3){-0.2f, -1.5f, -0.3f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){1.2f, 0.4f, 1.2f}
	);
	
	c_putpropat(
		R_MLIGHTBULB,
		R_TLIGHTBULB,
		'c',
		(vec3){0.0f, 0.5f, 0.0f},
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.5f, 0.2f, 0.5f}
	);
	
	// move camera.
	glm_vec3_copy((vec3){0.5f, 0.0f, 3.2f}, r_cam.base.pos);
	r_cam.pan.pitch = 0.0f; // reset from pan at game end.
	r_cam.base.yaw = -GLM_PI / 8.0f;
	
	// menu loop.
	bool startquick = false;
	for (;; m_cycle += O_MENUCYCLERATE)
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
		r_update();
		s_update();
		
		r_cam.base.pos[1] = O_MENUCYCLEBOB * sin(m_cycle);
		
		// do UI.
		u_begin(20, 20);
		if (u_button("Quit"))
		{
			exit(0);
		}
		if (u_button("Options"))
		{
			m_options();
			continue;
		}
		if (u_button("Skip intro sequence"))
		{
			startquick = true;
			break;
		}
		if (u_button("Play from beginning"))
		{
			break;
		}
		u_pad(0, 20);
		u_label("    A game by Tirimid");
		u_label("'Shame on Me'");
		
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
		ENDTIMER(stagetimer, "menus: render shadow");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_BASE);
		r_beginbase();
		c_rendertiles();
		c_rendermodels();
		ENDTIMER(stagetimer, "menus: render base");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		u_render();
		ENDTIMER(stagetimer, "menus: render overlay");
		
		BEGINTIMER(&stagetimer);
		i32 dw, dh;
		i32 vw, vh;
		r_renderbounds(&dw, &dh);
		r_textsize(R_VCROSDMONO, O_VERSION, &vw, &vh);
		r_rendertext(R_VCROSDMONO, O_VERSION, 5, dh - vh, vw, vh);
		ENDTIMER(stagetimer, "menus: misc render");
		
		ENDTIMER(largetimer, "menus: render");
		
		r_present();
		
		endtick();
	}
	
	if (!startquick)
	{
		m_intro();
	}
	g_loop(startquick);
	m_credits();
}

void
m_options(void)
{
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	char kselbtn[32] = {0};
	char kskipbtn[32] = {0};
	char krightbtn[32] = {0};
	char kleftbtn[32] = {0};
	
	for (;; m_cycle += O_MENUCYCLERATE)
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
		r_update();
		s_update();
		
		r_cam.base.pos[1] = O_MENUCYCLEBOB * sin(m_cycle);
		
		// compute UI key change labels.
		sprintf(kselbtn, "Select: %s", SDL_GetKeyName(o_dyn.ksel));
		sprintf(kskipbtn, "Skip: %s", SDL_GetKeyName(o_dyn.kskip));
		sprintf(krightbtn, "Right: %s", SDL_GetKeyName(o_dyn.kright));
		sprintf(kleftbtn, "Left: %s", SDL_GetKeyName(o_dyn.kleft));
		
		// do UI.
		u_begin(20, 20);
		if (u_button("Done"))
		{
			return;
		}
		u_pad(0, 20);
		if (u_slider("Music volume", &o_dyn.musicvolume))
		{
			// sound module already accounts for this in s_update() so no explicit
			// adjustment of music volume needs to happen here.
			o_dynwrite();
		}
		if (u_slider("SFX volume", &o_dyn.sfxvolume))
		{
			s_sfxvolume(o_dyn.sfxvolume);
			o_dynwrite();
			
			// play test SFX for user to understand volume.
			static u8 beeptick = 0;
			++beeptick;
			if (beeptick >= 10)
			{
				s_playsfx(S_BEEP);
				beeptick = 0;
			}
		}
		if (u_button(kleftbtn))
		{
			o_dyn.kleft = m_keyselect();
			o_dynwrite();
		}
		if (u_button(krightbtn))
		{
			o_dyn.kright = m_keyselect();
			o_dynwrite();
		}
		if (u_button(kskipbtn))
		{
			o_dyn.kskip = m_keyselect();
			o_dynwrite();
		}
		if (u_button(kselbtn))
		{
			o_dyn.ksel = m_keyselect();
			o_dynwrite();
		}
		if (u_button(o_dyn.pixelation < O_HIGHPIXELATION ? "Pixelation: low" : "Pixelation: high"))
		{
			o_dyn.pixelation = o_dyn.pixelation < O_HIGHPIXELATION ? O_HIGHPIXELATION : O_LOWPIXELATION;
			o_dynwrite();
			
			// refresh window size to regenerate render buffers.
			i32 w, h;
			SDL_GetWindowSize(r_wnd, &w, &h);
			r_resize(w, h);
			
			continue;
		}
		if (u_button(o_dyn.fullscreen ? "Fullscreen: on" : "Fullscreen: off"))
		{
			o_dyn.fullscreen = !o_dyn.fullscreen;
			o_dynwrite();
			
			SDL_SetWindowFullscreen(r_wnd, o_dyn.fullscreen * SDL_WINDOW_FULLSCREEN_DESKTOP);
			
			continue;
		}
		u_pad(0, 20);
		u_label("Options");
		
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
		ENDTIMER(stagetimer, "menus: render shadow");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_BASE);
		r_beginbase();
		c_rendertiles();
		c_rendermodels();
		ENDTIMER(stagetimer, "menus: render base");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		u_render();
		ENDTIMER(stagetimer, "menus: render overlay");
		
		ENDTIMER(largetimer, "menus: render");
		
		r_present();
		
		endtick();
	}
}

SDL_Keycode
m_keyselect(void)
{
	NEWTIMER(largetimer);
	NEWTIMER(stagetimer);
	
	for (;; m_cycle += O_MENUCYCLERATE)
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
		r_update();
		s_update();
		
		r_cam.base.pos[1] = O_MENUCYCLEBOB * sin(m_cycle);
		
		for (SDL_Keycode i = 0; i < 128; ++i)
		{
			if (i_kpressed(i))
			{
				return i;
			}
		}
		
		for (SDL_Keycode i = 128; i < 1024; ++i)
		{
			if (i_kpressed(i))
			{
				return (i - 128) | 1 << 30;
			}
		}
		
		// do UI.
		u_begin(20, 20);
		u_label("(Press a key)");
		u_pad(0, 20);
		u_label("Key select");
		
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
		ENDTIMER(stagetimer, "menus: render shadow");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_BASE);
		r_beginbase();
		c_rendertiles();
		c_rendermodels();
		ENDTIMER(stagetimer, "menus: render base");
		
		BEGINTIMER(&stagetimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		u_render();
		ENDTIMER(stagetimer, "menus: render overlay");
		
		ENDTIMER(largetimer, "menus: render");
		
		r_present();
		
		endtick();
	}
}

void
m_intro(void)
{
	NEWTIMER(rendertimer);
	
	char const *text[] =
	{
		"It is February of 2032.",
		"",
		"Within the past months, the world has seen an",
		"unprecedented rise of seismic events within",
		"the Earth's crust and upper mantle. Hundreds",
		"of millions have already perished as a result",
		"of earthquakes and volcanic activity.",
		"",
		"An extreme emergency order has been issued by",
		"the government of the country.",
		"",
		"Our main character is a young man by the name",
		"of Arkady. His friends have called him to",
		"visit their place on some unspecified",
		"occasion.",
		"",
		"He is currently heading over."
	};
	
	m_textscroll(text, sizeof(text) / sizeof(text[0]));
}

void
m_credits(void)
{
	NEWTIMER(rendertimer);
	
	char const *text[] =
	{
		"After leaving the building, Arkady ran home to",
		"get some much-needed rest and forget the",
		"troubles that the day had presented thus far.",
		"",
		"He had just seen his only friend group wipe",
		"itself out one-by-one, and was in no state to",
		"stay awake any longer without losing what",
		"little was left of his mind.",
		"",
		"Lying in bed, he thought about Matthew, Peter,",
		"and Gerasim. Arkady began to doze off as his",
		"body gave in to the exhaustion. Screams",
		"resonated in the distance, amplified by the",
		"shaking of the Earth as it ravaged the",
		"far-away apartment complexes. The screams were",
		"getting louder, they were getting closer.",
		"",
		"It didn't matter what happened anymore.",
		"",
		"Arkady finally fell asleep.",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"'Shame on Me'",
		"    A game by Tirimid",
		"",
		"              Development",
		"",
		"            Tirimid  Programming",
		"            Tirimid  Writing",
		"            Tirimid  Sound design",
		"            Tirimid  Textures",
		"            Tirimid  3D models",
		"          satsualex  Playtesting",
		"",
		"              Resources",
		"",
		"       Riciery Leal  VCR OSD Mono",
		"         SFX sounds  Brick wall broke",
		"         GFX Sounds  Thunder Rumble Sound Effect",
		"                SFX  Thunder Rumbling",
		" Sound Effects Yang  Craking Wall, Ceiling, Building",
		"1000s sound effects  Crumbling sound effect",
		"       u_b32baquv5u  8-Bit Explosion 3",
		"   Dominique Lacaud  Explosion",
		"      SonixFXSounds  Explosion",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Thanks for playing,",
		"The End."
	};
	
	m_textscroll(text, sizeof(text) / sizeof(text[0]));
}
	
static void
m_textscroll(char const *lines[], usize nlines)
{
	i32 height = O_MENUTEXTHEIGHT * nlines;
	height += 20; // add some dead time at the end.
	
	i32 rw, rh;
	r_renderbounds(&rw, &rh);
	
	char skiptext[64];
	sprintf(skiptext, "Press %s to skip", SDL_GetKeyName(o_dyn.kskip));
	
	i32 skiptextw, skiptexth;
	r_textsize(R_VCROSDMONO, skiptext, &skiptextw, &skiptexth);
	
	for (f32 scroll = -20.0f; scroll - height < rh; scroll += 0.8f / o_dyn.pixelation)
	{
		begintick();
		
		r_renderbounds(&rw, &rh);
		
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
		r_update();
		s_update();
		
		if (i_kpressed(o_dyn.kskip))
		{
			return;
		}
		
		// render.
		BEGINTIMER(&rendertimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (usize i = 0; i < nlines; ++i)
		{
			if (!strlen(lines[i]))
			{
				continue;
			}
			i32 tw, th;
			r_textsize(R_VCROSDMONO, lines[i], &tw, &th);
			r_rendertext(R_VCROSDMONO, lines[i], 40, scroll - O_MENUTEXTHEIGHT * i, tw, th);
		}
		r_renderrect(R_BLACK, rw - skiptextw, 0, skiptextw, skiptexth, 0.0f);
		r_rendertext(R_VCROSDMONO, skiptext, rw - skiptextw, 0, skiptextw, skiptexth);
		ENDTIMER(rendertimer, "menus: render");
		
		r_present();
		
		endtick();
	}
}
