#ifndef TEXTBOX_H
#define TEXTBOX_H

typedef enum t_sprite
{
	T_ARKADY = 0,
	T_PETER,
	T_MATTHEW,
	T_GERASIM,
	
	T_SPRITE_END__
} t_sprite;

bool t_active(void);
void t_show(t_sprite sprite, char const *msg);
void t_update(void);
void t_renderoverlay(void);

#endif
