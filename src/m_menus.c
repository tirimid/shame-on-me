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
		
		// update game systems.
		r_update();
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
	
	g_loop(startquick);
}

void
m_options(void)
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
		
		// update game systems.
		r_update();
		r_cam.base.pos[1] = O_MENUCYCLEBOB * sin(m_cycle);
		
		// compute UI key change labels.
		static char kselbtn[32] = {0};
		static char kskipbtn[32] = {0};
		static char krightbtn[32] = {0};
		static char kleftbtn[32] = {0};
		
		sprintf(kselbtn, "Select: %s", SDL_GetKeyName(o_dyn.ksel));
		sprintf(kskipbtn, "Skip: %s", SDL_GetKeyName(o_dyn.kskip));
		sprintf(krightbtn, "Right: %s", SDL_GetKeyName(o_dyn.kright));
		sprintf(kleftbtn, "Left: %s", SDL_GetKeyName(o_dyn.kleft));
		
		// do UI.
		u_begin(20, 20);
		if (u_button("Done"))
		{
			o_dynwrite();
			return;
		}
		u_label("(You may need to restart)");
		u_pad(0, 20);
		if (u_slider("Music volume", &o_dyn.musicvolume))
		{
			// TODO: implement music volume change realtime.
		}
		if (u_slider("SFX volume", &o_dyn.sfxvolume))
		{
			s_sfxvolume(o_dyn.sfxvolume);
		}
		if (u_button(kleftbtn))
		{
			o_dyn.kleft = m_keyselect();
		}
		if (u_button(krightbtn))
		{
			o_dyn.kright = m_keyselect();
		}
		if (u_button(kskipbtn))
		{
			o_dyn.kskip = m_keyselect();
		}
		if (u_button(kselbtn))
		{
			o_dyn.ksel = m_keyselect();
		}
		if (u_button(o_dyn.fullscreen ? "Disable fullscreen" : "Enable fullscreen"))
		{
			o_dyn.fullscreen = !o_dyn.fullscreen;
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
		
		// update game systems.
		r_update();
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
