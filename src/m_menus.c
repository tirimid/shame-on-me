// SPDX-License-Identifier: GPL-3.0-or-later

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
			// TODO: implement music volume change realtime.
		}
		if (u_slider("SFX volume", &o_dyn.sfxvolume))
		{
			s_sfxvolume(o_dyn.sfxvolume);
			o_dynwrite();
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
	
	char presskeymsg[64] = {0};
	sprintf(presskeymsg, "(Press %s to continue)", SDL_GetKeyName(o_dyn.ksel));
	
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
		r_update();
		s_update();
		
		if (i_kpressed(o_dyn.ksel))
		{
			return;
		}
		
		// do UI.
		u_begin(10, 10);
		u_label(presskeymsg);
		u_pad(0, 10);
		u_label("currently heading over.");
		u_label("on some unspecified occasion. You are");
		u_label("Your friends have called you to visit ASAP");
		u_pad(0, 10);
		u_label("by the government of your country.");
		u_label("An extreme emergency order has been issued");
		u_pad(0, 10);
		u_label("volcanic activity.");
		u_label("as a result of the earthquakes and");
		u_label("Hundreds of millions have already perished");
		u_label("within the Earth's crust and upper mantle.");
		u_label("an unprecedented rise of seismic events");
		u_label("Within the past months, the world has seen");
		u_pad(0, 10);
		u_label("It is February of 2032.");
		
		// render.
		BEGINTIMER(&rendertimer);
		r_setshader(R_OVERLAY);
		r_beginoverlay();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		u_render();
		ENDTIMER(rendertimer, "menus: render");
		
		r_present();
		
		endtick();
	}
}

void
m_credits(void)
{
	// TODO: implement credits menu.
}
