typedef enum d_ai
{
	D_AGGRESSIVE = 1,
	D_MERCIFUL,
	D_BALANCED
} d_ai;

d_gamestate d_state =
{
	.attacker = D_MATTHEW
};

static void d_aiattack(d_cardstack *src, d_ai ai);
static void d_aidefend(d_cardstack *src, d_ai ai);
static void d_rendercard(u8 card, f32 relx, f32 rely, f32 angle);

static u8 d_playerai[D_PLAYER_END__] =
{
	0, // arkady.
	D_AGGRESSIVE, // peter.
	D_MERCIFUL, // matthew.
	D_BALANCED // gerasim.
};

void
d_setphase(d_gamephase phase)
{
	d_state.gamephase = phase;
	switch (phase)
	{
	case D_START:
		d_state.draw.ncards = D_NMAXCARDS;
		for (i32 i = 0; i < D_NMAXCARDS; ++i)
		{
			u8 val = i % 9 + 1, suit = i % 4 + 1;
			d_state.draw.cards[i] = suit << D_SUITSHIFT | val << D_VALUESHIFT;
		}
		d_shuffle(&d_state.draw);
		break;
	case D_CHOOSETRUMP:
		for (i32 i = 0; i <= 4; ++i)
		{
			if ((d_state.draw.cards[i] & D_VALUEMASK) == D_A)
			{
				continue;
			}
			d_state.trumpcard = d_state.draw.cards[i];
			d_rmcard(&d_state.draw, i);
		}
		break;
	case D_DEALCARDS:
		for (i32 i = 0; i < D_NDEALCARDS * D_PLAYER_END__; ++i)
		{
			u8 deal = d_state.draw.cards[0];
			d_rmcard(&d_state.draw, 0);
			d_addcard(&d_state.players[i % D_PLAYER_END__], deal);
		}
		for (i32 i = 0; i < D_PLAYER_END__; ++i)
		{
			d_sort(&d_state.players[i]);
		}
		break;
	case D_ATTACK:
		// TODO: implement attack game phase.
		break;
	case D_DEFEND:
		// TODO: implement defend game phase.
		break;
	default:
		break;
	}
}

void
d_renderoverlay(void)
{
	if (!d_state.gamephase)
	{
		return;
	}
	
	if (d_state.trumpcard & D_VALUEMASK)
	{
		d_rendercard(d_state.trumpcard, 0.25f, 0.6f, 0.0f);
	}
	
	if (d_state.draw.ncards)
	{
		d_rendercard(0, 0.25f, 0.7f, GLM_PI / 2.0f);
	}
	
	if (d_state.covered.ncards)
	{
		d_rendercard(0, 0.55f, 0.7f, GLM_PI / 2.0f);
	}
	
	for (i32 i = 0; i < d_state.players[D_ARKADY].ncards; ++i)
	{
		f32 pos = (f32)i / d_state.players[D_ARKADY].ncards;
		pos = 0.25f + 0.5f * pos;
		d_rendercard(d_state.players[D_ARKADY].cards[i], pos, 0.0f, 0.0f);
	}
	
	for (i32 i = 0; i < d_state.players[D_PETER].ncards; ++i)
	{
		f32 pos = (f32)i / d_state.players[D_PETER].ncards;
		pos = 0.25f + 0.5f * pos;
		d_rendercard(0, 0.0f, pos, GLM_PI / 2.0f);
	}
	
	for (i32 i = 0; i < d_state.players[D_MATTHEW].ncards; ++i)
	{
		f32 pos = (f32)i / d_state.players[D_MATTHEW].ncards;
		pos = 0.25f + 0.5f * pos;
		d_rendercard(0, pos, 1.0f, 0.0f);
	}
	
	for (i32 i = 0; i < d_state.players[D_GERASIM].ncards; ++i)
	{
		f32 pos = (f32)i / d_state.players[D_GERASIM].ncards;
		pos = 0.25f + 0.5f * pos;
		d_rendercard(0, 1.0f, pos, GLM_PI / 2.0f);
	}
	
	// TODO: implement durak render overlay.
}

void
d_update(void)
{
	// TODO: implement durak update.
}

void
d_shuffle(d_cardstack *stack)
{
	for (i32 i = 0; i < stack->ncards; ++i)
	{
		usize other = randint(i, stack->ncards);
		u8 tmp = stack->cards[other];
		stack->cards[other] = stack->cards[i];
		stack->cards[i] = tmp;
	}
}

void
d_addcard(d_cardstack *stack, u8 card)
{
	if (stack->ncards < D_NMAXCARDS)
	{
		stack->cards[stack->ncards] = card;
		++stack->ncards;
	}
}

void
d_rmcard(d_cardstack *stack, usize idx)
{
	memmove(&stack->cards[idx], &stack->cards[idx + 1], --stack->ncards - idx);
}

void
d_sort(d_cardstack *stack)
{
	for (usize i = 0; i < stack->ncards; ++i)
	{
		for (usize j = 1; j < stack->ncards; ++j)
		{
			if (stack->cards[j] < stack->cards[j - 1])
			{
				u8 tmp = stack->cards[j];
				stack->cards[j] = stack->cards[j - 1];
				stack->cards[j - 1] = tmp;
			}
		}
	}
}

static void
d_aiattack(d_cardstack *src, d_ai ai)
{
	// TODO: implement AI attack.
}

static void
d_aidefend(d_cardstack *src, d_ai ai)
{
	// TODO: implement AI defend.
}

static void
d_rendercard(u8 card, f32 relx, f32 rely, f32 angle)
{
	i32 rw, rh;
	r_renderbounds(&rw, &rh);
	
	i32 absx = relx * rw - O_CARDWIDTH / 2;
	i32 absy = rely * rh - O_CARDWIDTH / 2;
	
	u8 tex = R_CBACK;
	if (card)
	{
		u8 suit = card & D_SUITMASK, value = card & D_VALUEMASK;
		switch (suit)
		{
		case D_SPADES:
			tex = R_CS6 + value - 1;
			break;
		case D_DIAMONDS:
			tex = R_CD6 + value - 1;
			break;
		case D_CLUBS:
			tex = R_CC6 + value - 1;
			break;
		case D_HEARTS:
			tex = R_CH6 + value - 1;
			break;
		default:
			break;
		}
	}
	
	r_renderrect(
		R_BLACK50,
		absx - O_CARDOUTLINE,
		absy - O_CARDOUTLINE,
		O_CARDWIDTH + 2 * O_CARDOUTLINE,
		O_CARDHEIGHT + 2 * O_CARDOUTLINE,
		angle
	);
	r_renderrect(tex, absx, absy, O_CARDWIDTH, O_CARDHEIGHT, angle);
}
