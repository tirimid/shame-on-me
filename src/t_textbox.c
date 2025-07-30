// SPDX-License-Identifier: GPL-3.0-or-later

static bool t_curactive = false;
static u8 t_cursprite;
static char const *t_msg;
static usize t_msglen;
static usize t_scroll;
static u16 t_voicetick;

// data tables.
static u8 t_spritetex[T_SPRITE_END] =
{
	R_SOMETHING, // arkady.
	R_EYESDUMMYFACE, // peter.
	R_GLASSESDUMMYFACE, // matthew.
	R_DUMMYFACE, // gerasim.
	R_SOMETHING, // mute arkady.
	R_EYESDUMMYFACE, // mute peter.
	R_GLASSESDUMMYFACE, // mute matthew.
	R_DUMMYFACE, // mute gerasim.
	R_SOMETHING // tutorial.
};

static u8 t_voices[T_SPRITE_END] =
{
	S_SPEAKARKADY, // arkady.
	S_SPEAKPETER, // peter.
	S_SPEAKMATTHEW, // matthew.
	S_SPEAKGERASIM, // gerasim.
	S_SFX_END, // mute arkady.
	S_SFX_END, // mute peter.
	S_SFX_END, // mute matthew.
	S_SFX_END, // mute gerasim.
	S_SFX_END // tutorial
};

bool
t_active(void)
{
	return t_curactive;
}

bool
t_scrolldone(void)
{
	return t_scroll >= t_msglen;
}

void
t_show(t_sprite_t sprite, char const *msg)
{
	if (t_curactive)
	{
		return;
	}
	
	t_curactive = true;
	t_cursprite = sprite;
	t_msg = msg;
	t_msglen = strlen(msg);
	t_voicetick = 0;
	
	if (t_voices[sprite] != S_SFX_END)
	{
		t_scroll = 1; // need to start at 1 char to not crash SDL2 TTF.
	}
	else
	{
		t_scroll = t_msglen;
	}
}

void
t_update(void)
{
	if (!t_curactive)
	{
		return;
	}
	
	// update scroll.
	t_scroll += O_TEXTSCROLL;
	t_scroll = t_scroll > t_msglen ? t_msglen : t_scroll;
	
	// update speech SFX.
	if (t_voices[t_cursprite] != S_SFX_END && t_scroll < t_msglen)
	{
		if (!t_voicetick)
		{
			s_playsfx(t_voices[t_cursprite]);
			t_voicetick = O_VOICETICK;
		}
		--t_voicetick;
	}
	
	// update textbox update.
	if (i_kpressed(o_dyn.ksel))
	{
		if (t_scroll < t_msglen)
		{
			t_scroll = t_msglen;
		}
		else
		{
			t_curactive = false;
		}
	}
}

void
t_renderoverlay(void)
{
	if (!t_curactive)
	{
		return;
	}
	
	i32 rw, rh;
	r_renderbounds(&rw, &rh);
	
	r_renderrect(R_BLACK50, 0, 0, rw, rh, 0.0f);
	r_renderrect(
		t_spritetex[t_cursprite],
		10,
		5 + O_TEXTBOXSIZE,
		O_TEXTSPRITESIZE,
		O_TEXTSPRITESIZE,
		0.0f
	);
	r_renderrect(R_BLACK, 10, 5, rw - 20, O_TEXTBOXSIZE, 0.0f);
	
	char *msg = malloc(t_scroll + 1);
	hmemcpy(msg, t_msg, t_scroll);
	msg[t_scroll] = 0;
	r_rendertext(R_VCROSDMONO, msg, 15, 10, rw - 30, O_TEXTBOXSIZE - 10);
	free(msg);
}
