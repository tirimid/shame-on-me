// SPDX-License-Identifier: GPL-3.0-or-later

// to represent a card, OR together a suit and a value.
// e.g. 7 of clubs is (D_CLUBS | D_7).

#define D_SPADES 0x1
#define D_DIAMONDS 0x2
#define D_CLUBS 0x3
#define D_HEARTS 0x4

#define D_6 0x1
#define D_7 0x2
#define D_8 0x3
#define D_9 0x4
#define D_10 0x5
#define D_J 0x6
#define D_Q 0x7
#define D_K 0x8
#define D_A 0x9

#define D_NMAXCARDS 36
#define D_NDEALCARDS 6

typedef enum d_player
{
	D_ARKADY = 0,
	D_PETER,
	D_MATTHEW,
	D_GERASIM,
	
	D_PLAYER_END
} d_player;

typedef enum d_gamephase
{
	D_START = 1,
	D_CHOOSETRUMP,
	D_DEALCARDS,
	D_ATTACK,
	D_DEFEND,
	D_FINISH
} d_gamephase;

typedef enum d_cardflag
{
	D_SHOW = 0x1,
	D_COVERUP = 0x2
} d_cardflag;

typedef struct d_carddata
{
	f32 x, y, rot;
	f32 dstx, dsty, dstrot;
	u16 flags;
	u8 suit, value;
} d_carddata;

typedef struct d_pcards
{
	u8 pcards[D_NMAXCARDS];
	u8 ncards;
} d_pcards;

typedef struct d_gamestate
{
	d_carddata data[D_NMAXCARDS];
	
	d_pcards players[D_PLAYER_END];
	d_pcards draw;
	d_pcards attack;
	d_pcards defend;
	d_pcards covered;
	
	u32 acttick;
	u8 losses[D_PLAYER_END];
	u8 trumpsuit;
	u8 attacker;
	u8 round, gamephase;
	u8 playersactive; // bit 0 = player 0 active, bit 1 = player 1, etc.
	u8 selidx;
} d_gamestate;

extern d_gamestate d_state;

void d_setphase(d_gamephase phase);
void d_renderoverlay(void);
void d_update(void);
