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

#define D_DEAD 0xff

typedef enum d_player
{
	D_ARKADY = 0,
	D_PETER,
	D_MATTHEW,
	D_GERASIM,
	
	D_PLAYER_END
} d_player_t;

typedef enum d_phase
{
	D_START = 1,
	D_CHOOSETRUMP,
	D_DEALCARDS,
	D_ATTACK,
	D_DEFEND,
	D_FINISH
} d_phase_t;

typedef enum d_cardflag
{
	D_SHOW = 0x1,
	D_COVERUP = 0x2
} d_cardflag_t;

typedef enum d_handpos
{
	// arkady's hand (always bottom) is drawn independently.
	// this is just a mechanism to allow the AI players to move around the table
	// and still maintain a logical rendered position for their card hands.
	D_LEFT = 1,
	D_TOP,
	D_RIGHT
} d_handpos;

typedef struct d_card
{
	f32 x, y, rot;
	f32 dstx, dsty, dstrot;
	u16 flags;
	u8 suit, value;
} d_card_t;

typedef struct d_pcards
{
	u8 pcards[D_NMAXCARDS];
	u8 ncards;
} d_pcards_t;

typedef struct d_state
{
	d_card_t data[D_NMAXCARDS];
	
	d_pcards_t players[D_PLAYER_END];
	d_pcards_t draw;
	d_pcards_t attack;
	d_pcards_t defend;
	d_pcards_t covered;
	
	u32 acttick;
	u8 losses[D_PLAYER_END]; // set to D_DEAD upon next round after loss.
	u8 handpos[D_PLAYER_END];
	u8 trumpsuit;
	u8 attacker;
	u8 round, phase;
	u8 selidx;
	u8 playersactive; // bit 0 = player 0 active, bit 1 = player 1, etc.
} d_state_t;

extern d_state_t d_state;

void d_setphase(d_phase_t phase);
void d_renderoverlay(void);
void d_update(void);
