#ifndef DURAK_H
#define DURAK_H

// to represent a card, OR together a suit and a value.
// e.g. 7 of clubs is (D_CLUBS | D_7).

#define D_SUIT_MASK 0x30
#define D_SPADES 0x0
#define D_HEARTS 0x10
#define D_CLUBS 0x20
#define D_DIAMONDS 0x30

#define D_VALUE_MASK 0xf
#define D_6 0x0
#define D_7 0x1
#define D_8 0x2
#define D_9 0x3
#define D_10 0x4
#define D_J 0x5
#define D_Q 0x6
#define D_K 0x7
#define D_A 0x8

typedef enum D_Player
{
	D_P_ARKADY = 0,
	D_P_PETER,
	D_P_MATTHEW,
	D_P_GERASIM
} D_Player;

typedef struct D_DurakState
{
	struct
	{
		u8 Cards[36]; // enough memory for max possible card count.
		u8 CardCnt;
	} Players[4];
	u8 Attacker;
} D_DurakState;

extern D_DurakState D_State;

void D_RenderCards(void);
void D_RenderOverlay(void);
void D_Update(void);

#endif
