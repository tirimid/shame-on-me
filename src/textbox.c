static bool TextboxActive = false;
static u8 TextboxSprite;
static char const *TextboxMsg;

// data tables.
static u8 SpriteTextures[TS_END__] =
{
	T_SOMETHING, // arkady.
	T_DUMMY_FACE, // peter.
	T_GLASSES_DUMMY_FACE, // matthew.
	T_DUMMY_FACE // gerasim.
};

bool
T_IsActive(void)
{
	return TextboxActive;
}

void
T_Show(enum TextboxSprite Ts, char const *Msg)
{
	TextboxActive = true;
	TextboxSprite = Ts;
	TextboxMsg = Msg;
}

void
T_Update(void)
{
	if (!TextboxActive)
		return;
	
	if (I_KeyPressed(O_KEY_NEXT))
		TextboxActive = false;
}

void
T_Render(void)
{
	i32 Rw, Rh;
	R_GetRenderBounds(&Rw, &Rh);
	
	R_Rect(T_BLACK50, 0, 0, Rw, Rh);
	R_Rect(
		SpriteTextures[TextboxSprite],
		5,
		5 + O_TEXT_BOX_SIZE,
		O_TEXT_SPRITE_SIZE,
		O_TEXT_SPRITE_SIZE
	);
	R_Rect(T_BLACK, 5, 5, Rw - 10, O_TEXT_BOX_SIZE);
	R_Text(F_VCR_OSD_MONO, TextboxMsg, 10, 10, Rw - 20, O_TEXT_BOX_SIZE - 10);
}
