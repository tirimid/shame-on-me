static bool TextboxActive = false;
static u8 TextboxSprite;
static char const *TextboxMsg;

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
	
	if (I_KeyPressed(CF_KEY_NEXT))
		TextboxActive = false;
}

void
T_Render(void)
{
	if (!TextboxActive)
		return;
	
	// TODO: implement textbox render.
}
