#ifndef DURAK_H
#define DURAK_H

// to represent a card, OR together a suit and a value.
// e.g. 7 of clubs is (D_CLUBS | D_7).

#define D_SUITMASK 0xf0
#define D_SUITSHIFT 4
#define D_SPADES 0x10
#define D_DIAMONDS 0x20
#define D_CLUBS 0x30
#define D_HEARTS 0x40

#define D_VALUEMASK 0xf
#define D_VALUESHIFT 0
#define D_6 0x1
#define D_7 0x2
#define D_8 0x3
#define D_9 0x4
#define D_10 0x5
#define D_J 0x6
#define D_Q 0x7
#define D_K 0x8
#define D_A 0x9

#define D_MAXCARDS 36

typedef enum d_player
{
	D_ARKADY = 0,
	D_PETER,
	D_MATTHEW,
	D_GERASIM
} d_player;

typedef enum d_gamephase
{
	D_NULL = 0,
	D_START,
	D_CHOOSETRUMP,
	D_DEALCARDS,
	D_ATTACK,
	D_DEFEND
} d_gamephase;

typedef struct d_cardstack
{
	u8 cards[D_MAXCARDS];
	u8 ncards;
} d_cardstack;

typedef struct d_gamestate
{
	d_cardstack players[4];
	d_cardstack draw;
	u8 ncovered;
	
	u8 attacker;
	u8 trumpcard;
	u8 gamephase;
} d_gamestate;

extern d_gamestate d_state;

void d_setphase(d_gamephase phase);
void d_renderoverlay(void);
void d_update(void);
void d_shuffle(d_cardstack *stack);
void d_addcard(d_cardstack *stack, u8 card);
void d_rmcard(d_cardstack *stack, usize idx);
void d_sort(d_cardstack *stack);

#endif
