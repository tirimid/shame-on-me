static bool T_Active = false;
static u8 T_ActiveSprite;
static char const *T_ActiveMsg;

// data tables.
static u8 T_SpriteTextures[T_TS_END__] =
{
	R_T_SOMETHING, // arkady.
	R_T_EYES_DUMMY_FACE, // peter.
	R_T_GLASSES_DUMMY_FACE, // matthew.
	R_T_DUMMY_FACE // gerasim.
};

bool
T_IsActive(void)
{
	return T_Active;
}

void
T_Show(T_TextboxSprite Sprite, char const *Msg)
{
	T_Active = true;
	T_ActiveSprite = Sprite;
	T_ActiveMsg = Msg;
}

void
T_Update(void)
{
	if (!T_Active) {return;}
	if (I_KeyPressed(O_KEY_NEXT)) {T_Active = false;}
}

void
T_RenderOverlay(void)
{
	i32 Rw, Rh;
	R_GetRenderBounds(&Rw, &Rh);
	
	R_RenderRect(R_T_BLACK50, 0, 0, Rw, Rh);
	R_RenderRect(
		T_SpriteTextures[T_ActiveSprite],
		5,
		5 + O_TEXT_BOX_SIZE,
		O_TEXT_SPRITE_SIZE,
		O_TEXT_SPRITE_SIZE
	);
	R_RenderRect(R_T_BLACK, 5, 5, Rw - 10, O_TEXT_BOX_SIZE);
	R_RenderText(R_F_VCR_OSD_MONO, T_ActiveMsg, 10, 10, Rw - 20, O_TEXT_BOX_SIZE - 10);
}
