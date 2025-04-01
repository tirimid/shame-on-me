#ifndef TEXTBOX_H
#define TEXTBOX_H

typedef enum T_TextboxSprite
{
	T_TS_ARKADY = 0,
	T_TS_PETER,
	T_TS_MATTHEW,
	T_TS_GERASIM,
	
	T_TS_END__
} T_TextboxSprite;

bool T_IsActive(void);
void T_Show(T_TextboxSprite Sprite, char const *Msg);
void T_Update(void);
void T_RenderOverlay(void);

#endif
