#ifndef TEXTBOX_H
#define TEXTBOX_H

enum TextboxSprite
{
	TS_ARKADY = 0,
	TS_PETER,
	TS_MATTHEW,
	TS_GERASIM,
	
	TS_END__
};

bool T_IsActive(void);
void T_Show(enum TextboxSprite Ts, char const *Msg);
void T_Update(void);
void T_Render(void);

#endif
