d_gamestate d_state;

static void d_setcardtex(u8 card);
static void d_renderstack(u8 ncards, vec3 pos, vec3 rot);

void
d_setphase(d_gamephase phase)
{
	d_state.gamephase = phase;
	switch (phase)
	{
	case D_START:
		// TODO: shuffle cards, create stacks, etc.
		break;
	default:
		break;
	}
}

void
d_rendercards(void)
{
	if (d_state.gamephase < D_START)
	{
		return;
	}
	
	d_setcardtex(0);
	d_renderstack(d_state.draw.ncards, (vec3)O_DRAWSTACKPOS, (vec3)O_DRAWSTACKROT);
	d_renderstack(d_state.ncovered, (vec3)O_COVEREDSTACKPOS, (vec3)O_COVEREDSTACKROT);
	
	// TODO: finish implement durak render cards.
}

void
d_renderoverlay(void)
{
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
	// TODO: implement shuffle stack.
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
		for (usize i = 1; i < stack->ncards; ++i)
		{
			if (stack->cards[i] < stack->cards[i - 1])
			{
				u8 tmp = stack->cards[i];
				stack->cards[i] = stack->cards[i - 1];
				stack->cards[i - 1] = tmp;
			}
		}
	}
}

static void
d_setcardtex(u8 card)
{
	if (!card)
	{
		r_settex(R_CBACK);
		return;
	}
	
	u8 suit = card & D_SUITMASK, value = card & D_VALUEMASK;
	switch (suit)
	{
	case D_SPADES:
		r_settex(R_CS6 + value - 1);
		break;
	case D_DIAMONDS:
		r_settex(R_CD6 + value - 1);
		break;
	case D_CLUBS:
		r_settex(R_CC6 + value - 1);
		break;
	case D_HEARTS:
		r_settex(R_CH6 + value - 1);
		break;
	default:
		break;
	}
}

static void
d_renderstack(u8 ncards, vec3 pos, vec3 rot)
{
	if (!ncards)
	{
		return;
	}
	
	vec3 scale = O_CARDSTACKSCALE;
	scale[1] /= 37 - ncards;
	r_rendermodel(R_CARDSTACK, pos, rot, scale);
}
