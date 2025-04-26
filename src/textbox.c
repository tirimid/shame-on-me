static bool t_curactive = false;
static u8 t_cursprite;
static char const *t_curmsg;
static usize t_curmsglen;
static usize t_curscroll;

// data tables.
static u8 t_spritetex[T_SPRITE_END] =
{
	R_SOMETHING, // arkady.
	R_EYESDUMMYFACE, // peter.
	R_GLASSESDUMMYFACE, // matthew.
	R_DUMMYFACE // gerasim.
};

bool
t_active(void)
{
	return t_curactive;
}

void
t_show(t_sprite sprite, char const *msg)
{
	if (t_curactive)
	{
		return;
	}
	
	t_curactive = true;
	t_cursprite = sprite;
	t_curmsg = msg;
	t_curmsglen = strlen(msg);
	t_curscroll = 1; // need to start at 1 char to not crash SDL2 TTF.
}

void
t_update(void)
{
	if (!t_curactive)
	{
		return;
	}
	
	t_curscroll += O_TEXTSCROLL;
	t_curscroll = t_curscroll > t_curmsglen ? t_curmsglen : t_curscroll;
	
	if (i_kpressed(O_KSEL))
	{
		if (t_curscroll < t_curmsglen)
		{
			t_curscroll = t_curmsglen;
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
	
	char *msg = malloc(t_curscroll + 1);
	memcpy(msg, t_curmsg, t_curscroll);
	msg[t_curscroll] = 0;
	r_rendertext(R_VCROSDMONO, msg, 15, 10, rw - 30, O_TEXTBOXSIZE - 10);
	free(msg);
}
