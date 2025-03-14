#ifndef TEXTBOX_H
#define TEXTBOX_H

enum TextboxSprite
{
	TS_DUMMY = 0,
	
	TS_END__
};

void T_Show(enum TextboxSprite Ts, char const *Msg);
void T_Update(void);
void T_Render(void);

#endif
