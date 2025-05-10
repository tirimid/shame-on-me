// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum t_sprite
{
	T_ARKADY = 0,
	T_PETER,
	T_MATTHEW,
	T_GERASIM,
	T_MUTEARKADY,
	T_MUTEPETER,
	T_MUTEMATTHEW,
	T_MUTEGERASIM,
	
	T_SPRITE_END
} t_sprite_t;

bool t_active(void);
bool t_scrolldone(void);
void t_show(t_sprite_t sprite, char const *msg);
void t_update(void);
void t_renderoverlay(void);
