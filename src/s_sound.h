// SPDX-License-Identifier: GPL-3.0-or-later

typedef enum s_sfx
{
	S_SPEAKARKADY = 0,
	S_SPEAKGERASIM,
	S_SPEAKMATTHEW,
	S_SPEAKPETER,
	S_CARD0,
	S_CARD1,
	S_CARD2,
	S_CARD3,
	S_CARD4,
	S_CARD5,
	S_CARD6,
	S_CARD7,
	S_CARD8,
	S_CARDSHUFFLE,
	S_EXPLODE,
	S_EXPLODEMUFFLED,
	S_KNOCK,
	
	S_SFX_END
} s_sfx_t;

i32 s_init(void);
void s_quit(void);
void s_sfxvolume(f32 vol);
void s_playsfx(s_sfx_t id);
