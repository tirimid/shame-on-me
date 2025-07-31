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
		
		ENDTIMER(&largetimer, "menus: render");
		
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
		
		// do UI.
		u_begin(20, 20);
		if (u_button("Back"))
		{
			return;
		}
		u_pad(0, 20);
		u_label("Options will go here...");
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
		
		ENDTIMER(&largetimer, "menus: render");
		
		r_present();
		
		endtick();
	}
}
