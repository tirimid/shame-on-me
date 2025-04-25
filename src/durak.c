#define D_BALANCEDYIELD 35
#define D_BALANCEDHIATTACK 40

typedef enum d_ai
{
	D_AGGRESSIVE = 1,
	D_MERCIFUL,
	D_BALANCED
} d_ai;

d_gamestate d_state =
{
	.attacker = D_MATTHEW,
	.playersactive = 0xf,
	.selidx = D_NDEALCARDS
};

static void d_aiattack(d_cardstack *src, d_ai ai);
static void d_aidefend(d_cardstack *src);
static void d_playattack(d_cardstack *src);
static void d_playdefend(d_cardstack *src);
static void d_rendercard(u8 card, f32 relx, f32 rely, f32 angle);
static i32 d_power(u8 card);
static i32 d_powercmp(u8 lhs, u8 rhs);
static i32 d_next(i32 from);
static void d_allcovered(void);
static void d_takeall(void);
static void d_drawto(d_cardstack *dst, usize ncards);
static i32 d_reattack(d_cardstack *src, bool hiattack);
static i32 d_activeplayers(void);

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
		for (usize i = 0; i < D_PLAYER_END__; ++i)
		{
			d_drawto(&d_state.players[i], D_NDEALCARDS);
		}
		break;
	case D_ATTACK:
		d_state.acttick = O_ATTACKTICK + randint(0, O_VARYTICK);
		break;
	case D_DEFEND:
		d_state.acttick = O_DEFENDTICK + randint(0, O_VARYTICK);
		break;
	case D_FINISH:
		// TODO: implement durak finish.
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
	
	// render trump card.
	if (d_state.trumpcard & D_VALUEMASK)
	{
		d_rendercard(d_state.trumpcard, 0.25f, 0.6f, 0.0f);
	}
	
	// render draw stack.
	if (d_state.draw.ncards)
	{
		d_rendercard(0, 0.25f, 0.7f, GLM_PI / 2.0f);
	}
	
	// render covered stack.
	if (d_state.covered.ncards)
	{
		d_rendercard(0, 0.55f, 0.7f, GLM_PI / 2.0f);
	}
	
	// render AI player hands.
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
	
	// render active cards.
	for (i32 i = 0; i < d_state.active.ncards; i += 2)
	{
		f32 pos = (f32)(i / 2) / d_state.active.ncards;
		pos = 0.4f + 0.45f * pos;
		d_rendercard(d_state.active.cards[i], pos, 0.42f, 0.0f);
		if (d_state.active.cards[i + 1])
		{
			d_rendercard(d_state.active.cards[i + 1], pos, 0.36f, 0.0f);
		}
	}
	
	// render arkady's hand.
	for (i32 i = 0; i < d_state.players[D_ARKADY].ncards; ++i)
	{
		f32 posx = (f32)i / d_state.players[D_ARKADY].ncards;
		posx = 0.25f + 0.5f * posx;
		
		f32 posy = d_state.selidx == i ? 0.1f : 0.0f;
		
		d_rendercard(d_state.players[D_ARKADY].cards[i], posx, posy, 0.0f);
	}
}

void
d_update(void)
{
	// update game state.
	switch (d_state.gamephase)
	{
	case D_ATTACK:
		if (!d_playerai[d_state.attacker])
		{
			d_playattack(&d_state.players[d_state.attacker]);
		}
		else
		{
			--d_state.acttick;
			if (!d_state.acttick)
			{
				d_aiattack(&d_state.players[d_state.attacker], d_playerai[d_state.attacker]);
			}
		}
		break;
	case D_DEFEND:
	{
		i32 def = d_next(d_state.attacker);
		if (def == -1)
		{
			break;
		}
		
		if (!d_playerai[def])
		{
			d_playdefend(&d_state.players[def]);
		}
		else
		{
			--d_state.acttick;
			if (!d_state.acttick)
			{
				d_aidefend(&d_state.players[def]);
			}
		}
		break;
	}
	default:
		break;
	}
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
			if (d_power(stack->cards[j]) < d_power(stack->cards[j - 1]))
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
	if (d_state.active.ncards)
	{
		switch (ai)
		{
		case D_AGGRESSIVE:
		{
			i32 idx = d_reattack(src, true);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_MERCIFUL:
		{
			if (d_state.active.ncards >= 6)
			{
				d_allcovered();
				break;
			}
			
			i32 idx = d_reattack(src, false);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_BALANCED:
		{
			if (randint(0, 100) < D_BALANCEDYIELD)
			{
				d_allcovered();
				break;
			}
			
			i32 idx = d_reattack(src, randint(0, 100) < D_BALANCEDHIATTACK);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		}
	}
	else
	{
		switch (ai)
		{
		case D_AGGRESSIVE:
		{
			i32 power = 0;
			usize idx = 0;
			for (usize i = 0; i < src->ncards; ++i)
			{
				if (d_power(src->cards[i]) > power)
				{
					power = d_power(src->cards[i]);
					idx = i;
				}
			}
			
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_MERCIFUL:
		{
			i32 power = 1000;
			usize idx = 0;
			for (usize i = 0; i < src->ncards; ++i)
			{
				if (d_power(src->cards[i]) < power)
				{
					power = d_power(src->cards[i]);
					idx = i;
				}
			}
			
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_BALANCED:
		{
			usize idx = randint(0, src->ncards);
			u8 card = src->cards[idx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, idx);
			
			d_setphase(D_DEFEND);
			
			break;
		}
		}
	}
}

static void
d_aidefend(d_cardstack *src)
{
	if (!src->ncards)
	{
		d_takeall();
		return;
	}
	
	for (usize i = 0; i < d_state.active.ncards; i += 2)
	{
		if (d_state.active.cards[i + 1])
		{
			continue;
		}
		
		i32 idx = -1;
		i32 power = 1000;
		for (i32 j = 0; j < src->ncards; ++j)
		{
			if (d_powercmp(src->cards[j], d_state.active.cards[i]) <= 0)
			{
				continue;
			}
			
			if (d_power(src->cards[j]) < power)
			{
				power = d_power(src->cards[j]);
				idx = j;
			}
		}
		
		if (idx == -1)
		{
			d_takeall();
			return;
		}
		
		d_state.active.cards[i + 1] = src->cards[idx];
		d_rmcard(src, idx);
	}
	
	d_setphase(D_ATTACK);
}

static void
d_playattack(d_cardstack *src)
{
	d_state.selidx = d_state.selidx >= src->ncards ? 0 : d_state.selidx;
	
	if (i_kpressed(O_KRIGHT))
	{
		d_state.selidx = d_state.selidx + 1 >= src->ncards ? 0 : d_state.selidx + 1;
	}
	
	if (i_kpressed(O_KLEFT))
	{
		d_state.selidx = d_state.selidx == 0 ? src->ncards - 1 : d_state.selidx - 1;
	}
	
	if (d_state.active.ncards)
	{
		if (src->ncards && i_kpressed(O_KSEL))
		{
			u8 card = src->cards[d_state.selidx];
			
			for (usize i = 0; i < d_state.active.ncards; ++i)
			{
				if ((d_state.active.cards[i] & D_VALUEMASK) == (card & D_VALUEMASK))
				{
					d_addcard(&d_state.active, card);
					d_addcard(&d_state.active, 0);
					d_rmcard(src, d_state.selidx);
					
					d_state.selidx = -1;
					d_setphase(D_DEFEND);
					
					break;
				}
			}
		}
		else if (i_kpressed(O_KSKIP))
		{
			d_state.selidx = -1;
			d_allcovered();
		}
	}
	else
	{
		if (i_kpressed(O_KSEL))
		{
			u8 card = src->cards[d_state.selidx];
			
			d_addcard(&d_state.active, card);
			d_addcard(&d_state.active, 0);
			d_rmcard(src, d_state.selidx);
			
			d_state.selidx = -1;
			d_setphase(D_DEFEND);
		}
	}
}

static void
d_playdefend(d_cardstack *src)
{
	d_state.selidx = d_state.selidx >= src->ncards ? 0 : d_state.selidx;
	
	if (i_kpressed(O_KRIGHT))
	{
		d_state.selidx = d_state.selidx + 1 >= src->ncards ? 0 : d_state.selidx + 1;
	}
	
	if (i_kpressed(O_KLEFT))
	{
		d_state.selidx = d_state.selidx == 0 ? src->ncards - 1 : d_state.selidx - 1;
	}
	
	if (src->ncards && i_kpressed(O_KSEL))
	{
		u8 card = src->cards[d_state.selidx];
		for (usize i = 0; i < d_state.active.ncards; i += 2)
		{
			if (d_state.active.cards[i + 1])
			{
				continue;
			}
			
			if (d_powercmp(card, d_state.active.cards[i]) > 0)
			{
				d_state.active.cards[i + 1] = card;
				d_rmcard(src, d_state.selidx);
				
				d_state.selidx = -1;
				d_setphase(D_ATTACK);
				
				break;
			}
		}
	}
	else if (i_kpressed(O_KSKIP))
	{
		d_state.selidx = -1;
		d_takeall();
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
		R_BLACK50,
		absx - O_CARDOUTLINE,
		absy - O_CARDOUTLINE,
		O_CARDWIDTH + 2 * O_CARDOUTLINE,
		O_CARDHEIGHT + 2 * O_CARDOUTLINE,
		angle
	);
	r_renderrect(tex, absx, absy, O_CARDWIDTH, O_CARDHEIGHT, angle);
}

static i32
d_power(u8 card)
{
	i32 val = card & D_VALUEMASK;
	val += 256 * ((card & D_SUITMASK) == (d_state.trumpcard & D_SUITMASK));
	return val;
}

static i32
d_powercmp(u8 lhs, u8 rhs)
{
	u8 ls = lhs & D_SUITMASK, lv = lhs & D_VALUEMASK;
	u8 rs = rhs & D_SUITMASK, rv = rhs & D_VALUEMASK;
	u8 ts = d_state.trumpcard & D_SUITMASK;
	
	if (ls == ts && rs != ts)
	{
		return 1;
	}
	else if (ls != ts && rs == ts)
	{
		return -1;
	}
	else if (ls == rs)
	{
		return lv - rv;
	}
	else
	{
		// non-comparable cards.
		return 0;
	}
}

static i32
d_next(i32 from)
{
	// allow chaining of d_next.
	if (from == -1)
	{
		return -1;
	}
	
	for (i32 i = 1; i <= D_PLAYER_END__; ++i)
	{
		u8 player = (from + i) % D_PLAYER_END__;
		if (d_state.playersactive & 1 << player)
		{
			return player;
		}
	}
	
	return -1;
}

static void
d_allcovered(void)
{
	i32 next = d_next(d_state.attacker);
	if (next == -1)
	{
		return;
	}
	
	// transfer active cards to covered pile.
	while (d_state.active.ncards)
	{
		u8 card = d_state.active.cards[0];
		if (card)
		{
			d_addcard(&d_state.covered, card);
		}
		d_rmcard(&d_state.active, 0);
	}
	
	// attacker draws to 6.
	d_drawto(&d_state.players[d_state.attacker], D_NDEALCARDS);
	if (!d_state.players[d_state.attacker].ncards)
	{
		d_state.playersactive &= ~(1 << d_state.attacker);
	}
	
	// defender draws to 6.
	d_drawto(&d_state.players[next], D_NDEALCARDS);
	if (!d_state.players[next].ncards)
	{
		d_state.playersactive &= ~(1 << d_state.attacker);
	}
	
	// advance attacker.
	next = d_next(d_state.attacker);
	if (next == -1)
	{
		return;
	}
	d_state.attacker = next;
	
	d_setphase(d_activeplayers() == 1 ? D_FINISH : D_ATTACK);
}

static void
d_takeall(void)
{
	i32 next = d_next(d_state.attacker);
	if (next == -1)
	{
		return;
	}
	
	// transfer active cards to defender.
	while (d_state.active.ncards)
	{
		u8 card = d_state.active.cards[0];
		if (card)
		{
			d_addcard(&d_state.players[next], card);
		}
		d_rmcard(&d_state.active, 0);
	}
	
	// attacker draws to deal cards.
	d_drawto(&d_state.players[d_state.attacker], D_NDEALCARDS);
	if (!d_state.players[d_state.attacker].ncards)
	{
		d_state.playersactive &= ~(1 << d_state.attacker);
	}
	
	// defender draws to deal cards.
	d_drawto(&d_state.players[next], D_NDEALCARDS);
	
	// advance attacker.
	next = d_next(next);
	if (next == -1)
	{
		return;
	}
	d_state.attacker = next;
	
	d_setphase(d_activeplayers() == 1 ? D_FINISH : D_ATTACK);
}

static void
d_drawto(d_cardstack *dst, usize ncards)
{
	while (dst->ncards < ncards && d_state.draw.ncards)
	{
		d_addcard(dst, d_state.draw.cards[0]);
		d_rmcard(&d_state.draw, 0);
	}
	
	if (dst->ncards < ncards && d_state.trumpcard & D_VALUEMASK)
	{
		d_addcard(dst, d_state.trumpcard);
		d_state.trumpcard &= ~D_VALUEMASK;
	}
	
	d_sort(dst);
}

static i32
d_reattack(d_cardstack *src, bool hiattack)
{
	if (hiattack)
	{
		for (i32 i = src->ncards - 1; i >= 0; --i)
		{
			for (usize j = 0; j < d_state.active.ncards; ++j)
			{
				if ((d_state.active.cards[j] & D_VALUEMASK) == (src->cards[i] & D_VALUEMASK))
				{
					return i;
				}
			}
		}
	}
	else
	{
		for (i32 i = 0; i < src->ncards; ++i)
		{
			for (usize j = 0; j < d_state.active.ncards; ++j)
			{
				if ((d_state.active.cards[j] & D_VALUEMASK) == (src->cards[i] & D_VALUEMASK))
				{
					return i;
				}
			}
		}
	}
	
	return -1;
}

static i32
d_activeplayers(void)
{
	i32 sum = 0;
	for (usize i = 0; i < 8; ++i)
	{
		sum += !!(d_state.playersactive & 1 << i);
	}
	return sum;
}
