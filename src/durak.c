d_gamestate d_state;

static void d_rendercard(u8 card, f32 relx, f32 rely, f32 angle);

void
d_setphase(d_gamephase phase)
{
	d_state.gamephase = phase;
	switch (phase)
	{
	case D_START:
		// create draw stack.
		d_state.draw.ncards = D_MAXCARDS;
		for (i32 i = 0; i < D_MAXCARDS; ++i)
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
	default:
		break;
	}
}

void
d_renderoverlay(void)
{
	if (d_state.gamephase < D_START)
	{
		return;
	}
	
	if (d_state.gamephase >= D_CHOOSETRUMP)
	{
		d_rendercard(d_state.trumpcard, 0.2f, 0.5f, 0.0f);
	}
	
	if (d_state.draw.ncards)
	{
		d_rendercard(0, 0.2f, 0.6f, GLM_PI / 2.0f);
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
	for (usize i = 0; i < stack->ncards; ++i)
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
	if (stack->ncards < D_MAXCARDS)
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
		R_BLACK,
		absx - O_CARDOUTLINE,
		absy - O_CARDOUTLINE,
		O_CARDWIDTH + 2 * O_CARDOUTLINE,
		O_CARDHEIGHT + 2 * O_CARDOUTLINE,
		angle
	);
	r_renderrect(tex, absx, absy, O_CARDWIDTH, O_CARDHEIGHT, angle);
}
