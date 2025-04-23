static bool t_curactive = false;
static u8 t_cursprite;
static char const *t_curmsg;

// data tables.
static u8 t_spritetex[T_SPRITE_END__] =
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
}

void
t_update(void)
{
	if (!t_curactive)
	{
		return;
	}
	
	if (i_kpressed(O_KNEXT))
	{
		t_curactive = false;
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
		5,
		5 + O_TEXTBOXSIZE,
		O_TEXTSPRITESIZE,
		O_TEXTSPRITESIZE,
		0.0f
	);
	r_renderrect(R_BLACK, 5, 5, rw - 10, O_TEXTBOXSIZE, 0.0f);
	r_rendertext(R_VCROSDMONO, t_curmsg, 10, 10, rw - 20, O_TEXTBOXSIZE - 10);
}
