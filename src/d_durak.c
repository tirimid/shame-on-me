// SPDX-License-Identifier: GPL-3.0-or-later

#define D_BALANCEDYIELD 35
#define D_BALANCEDHIATTACK 40
#define D_BALANCEDMID 2

typedef enum d_ai
{
	D_AGGRESSIVE = 1,
	D_MERCIFUL,
	D_BALANCED
} d_ai_t;

d_state_t d_state =
{
	.attacker = D_MATTHEW,
	.handpos = {0, D_LEFT, D_TOP, D_RIGHT},
	.playersactive = 0xf,
	.selidx = -1
};

static void d_aiattack(d_pcards_t *pc, d_ai_t ai);
static void d_aidefend(d_pcards_t *pc);
static void d_playattack(d_pcards_t *pc);
static void d_playdefend(d_pcards_t *pc);
static i32 d_power(u8 pcard);
static i32 d_powercmp(u8 plhs, u8 prhs);
static i32 d_nextplayer(i32 from);
static void d_allcovered(void);
static void d_takeall(void);
static void d_drawto(d_pcards_t *pc, usize ncards);
static i32 d_reattack(d_pcards_t *pc, bool hiattack);
static i32 d_activeplayers(void);
static void d_updatecard(d_card_t *card);
static void d_shuffle(d_pcards_t *pc);
static void d_addcard(d_pcards_t *pc, u8 pcard);
static void d_rmcard(d_pcards_t *pc, usize idx);
static void d_sort(d_pcards_t *pc);
static void d_rendercard(d_card_t const *card);
static void d_randcardsfx(void);

static u8 d_playerai[D_PLAYER_END] =
{
	0, // arkady.
	D_AGGRESSIVE, // peter.
	D_MERCIFUL, // matthew.
	D_BALANCED // gerasim.
};

void
d_setphase(d_phase_t phase)
{
	d_state.phase = phase;
	switch (phase)
	{
	case D_START:
		for (usize i = 0; i < D_NMAXCARDS; ++i)
		{
			d_state.data[i] = (d_card_t)
			{
				.x = 0.5f,
				.y = 1.5f,
				.suit = i % 4 + 1,
				.value = i % 9 + 1,
				.flags = D_SHOW
			};
		}
		
		for (usize i = 0; i < D_NMAXCARDS; ++i)
		{
			d_addcard(&d_state.draw, i);
		}
		d_shuffle(&d_state.draw);
		s_playsfx(S_CARDSHUFFLE);
		
		d_state.playersactive = 0;
		for (usize i = 0; i < D_PLAYER_END; ++i)
		{
			if (d_state.losses[i] < O_DEATHLOSSES)
			{
				d_state.playersactive |= 1 << i;
			}
			else
			{
				d_state.losses[i] = D_DEAD;
				d_state.handpos[i] = 0;
			}
		}
		
		++d_state.round;
		
		for (usize i = 0; i < D_PLAYER_END; ++i)
		{
			if (d_state.handpos[i] == D_TOP)
			{
				d_state.attacker = i;
				goto chosefirstattacker;
			}
		}
		
		d_state.attacker = d_nextplayer(D_PETER);
	chosefirstattacker:
		
		break;
	case D_CHOOSETRUMP:
	{
		d_card_t const *trump = &d_state.data[d_state.draw.pcards[D_NMAXCARDS - 1]];
		d_state.trumpsuit = trump->suit;
		d_randcardsfx();
		break;
	}
	case D_DEALCARDS:
		for (usize i = 0; i < D_PLAYER_END; ++i)
		{
			if (d_state.playersactive & 1 << i)
			{
				d_drawto(&d_state.players[i], D_NDEALCARDS);
			}
		}
		d_randcardsfx();
		break;
	case D_ATTACK:
		d_state.acttick = O_ATTACKTICK + randint(0, O_VARYTICK);
		break;
	case D_DEFEND:
		d_state.acttick = O_DEFENDTICK + randint(0, O_VARYTICK);
		break;
	case D_FINISH:
		d_state.draw.ncards = 0;
		d_state.covered.ncards = 0;
		d_state.attack.ncards = 0;
		d_state.defend.ncards = 0;
		d_randcardsfx();
		
		for (usize i = 0; i < D_PLAYER_END; ++i)
		{
			d_state.players[i].ncards = 0;
			if (d_state.playersactive & 1 << i)
			{
				++d_state.losses[i];
			}
		}
		
		for (usize i = 0; i < D_NMAXCARDS; ++i)
		{
			d_state.data[i].dstx = 0.5f;
			d_state.data[i].dsty = 1.5f;
			d_state.data[i].dstrot = 0.0f;
		}
		
		g_roundendseq();
		
		break;
	default:
		break;
	}
}

void
d_renderoverlay(void)
{
	// render draw stack.
	for (i32 i = d_state.draw.ncards - 1; i >= 0; --i)
	{
		d_rendercard(&d_state.data[d_state.draw.pcards[i]]);
	}
	
	// render covered stack.
	for (usize i = 0; i < d_state.covered.ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.covered.pcards[i]]);
	}
	
	// render AI player hands.
	for (usize i = 0; i < d_state.players[D_PETER].ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.players[D_PETER].pcards[i]]);
	}
	
	for (usize i = 0; i < d_state.players[D_MATTHEW].ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.players[D_MATTHEW].pcards[i]]);
	}
	
	for (usize i = 0; i < d_state.players[D_GERASIM].ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.players[D_GERASIM].pcards[i]]);
	}
	
	// render attack / defend cards.
	for (usize i = 0; i < d_state.attack.ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.attack.pcards[i]]);
		if (i < d_state.defend.ncards)
		{
			d_rendercard(&d_state.data[d_state.defend.pcards[i]]);
		}
	}
	
	// render Arkady's hand.
	for (usize i = 0; i < d_state.players[D_ARKADY].ncards; ++i)
	{
		d_rendercard(&d_state.data[d_state.players[D_ARKADY].pcards[i]]);
	}
	
	// at end of round, all cards need to be drawn leaving the screen.
	if (d_state.phase >= D_FINISH)
	{
		for (usize i = 0; i < D_NMAXCARDS; ++i)
		{
			d_rendercard(&d_state.data[i]);
		}
	}
	else if (d_state.phase >= D_ATTACK)
	{
		// do help UI.
		u_begin(10, 10);
		if (u_holdbutton("Rules"))
		{
			u_pad(0, 20);
			u_label("any other suit)");
			u_label("other, except for trump cards, which can be played against");
			u_label("(Only cards of equivalent suit can be played against each");
			u_label("  6 7 8 9 10 J Q K A");
			u_label("Card value (ascending):");
			u_pad(0, 20);
			u_label("You must get rid of your cards by the end to avoid losing.");
			u_label("and the defender begins a new attack on his left player.");
			u_label("Once the attack finishes, everyone draws back up to 6 cards");
			u_label("equal face value to any of the previously played cards.");
			u_label("for the defender to cover. This extra card must have an");
			u_label("attacker may either finish the attack, or add another card");
			u_label("cards in play and skip his turn. If he does defend, the");
			u_label("card over top of it. If he cannot defend, he must take all");
			u_label("as the 'defender', must 'defend' by placing a higher-value");
			u_label("left of him by placing out a card. The left player, known");
			u_label("cards. The first player begins an 'attack' on the player");
			u_label("chosen as the trump suit. Then, each player is dealt 6");
			u_label("At the start, a card is taken from the top and its suit is");
		}
		u_render();
	}
}

void
d_update(void)
{
	// only intended for debug / dev purposes.
	// may break shit if used incorrectly.
#ifdef CHEATING
	if (i_kpressed(SDLK_1) && d_state.playersactive & 0x1)
	{
		d_state.playersactive &= 0x1;
		d_setphase(D_FINISH);
		return;
	}
	else if (i_kpressed(SDLK_2) && d_state.playersactive & 0x2)
	{
		d_state.playersactive &= 0x2;
		d_setphase(D_FINISH);
		return;
	}
	else if (i_kpressed(SDLK_3) && d_state.playersactive & 0x4)
	{
		d_state.playersactive &= 0x4;
		d_setphase(D_FINISH);
		return;
	}
	else if (i_kpressed(SDLK_4) && d_state.playersactive & 0x8)
	{
		d_state.playersactive &= 0x8;
		d_setphase(D_FINISH);
		return;
	}
#endif
	
	// update card stack destinations.
	for (usize i = 0; i < d_state.draw.ncards; ++i)
	{
		u8 pcard = d_state.draw.pcards[i];
		if (d_state.phase >= D_CHOOSETRUMP && i + 1 == d_state.draw.ncards)
		{
			d_state.data[pcard].flags &= ~D_COVERUP;
			d_state.data[pcard].dstx = 0.25f;
			d_state.data[pcard].dsty = 0.6f;
			d_state.data[pcard].dstrot = 0.0f;
		}
		else
		{
			d_state.data[pcard].flags |= D_COVERUP;
			d_state.data[pcard].dstx = 0.25f;
			d_state.data[pcard].dsty = 0.7f;
			d_state.data[pcard].dstrot = GLM_PI / 2.0f;
		}
	}
	
	for (usize i = 0; i < d_state.covered.ncards; ++i)
	{
		u8 pcard = d_state.covered.pcards[i];
		
		d_state.data[pcard].flags |= D_COVERUP;
		d_state.data[pcard].dstx = 0.55f;
		d_state.data[pcard].dsty = 0.7f;
		d_state.data[pcard].dstrot = GLM_PI / 2.0f;
	}
	
	for (usize i = 0; i < d_state.players[D_ARKADY].ncards; ++i)
	{
		u8 pcard = d_state.players[D_ARKADY].pcards[i];
		
		d_state.data[pcard].flags &= ~D_COVERUP;
		d_state.data[pcard].dstx = 0.25f + 0.5f * ((f32)i / d_state.players[D_ARKADY].ncards);
		d_state.data[pcard].dsty = d_state.selidx == i ? 0.1f : 0.0f;
		d_state.data[pcard].dstrot = 0.0f;
	}
	
	for (usize i = D_ARKADY + 1; i < D_PLAYER_END; ++i)
	{
		for (usize j = 0; j < d_state.players[i].ncards; ++j)
		{
			u8 pcard = d_state.players[i].pcards[j];
			d_state.data[pcard].flags |= D_COVERUP;
			
			switch (d_state.handpos[i])
			{
			case D_LEFT:
				d_state.data[pcard].dstx = 0.0f;
				d_state.data[pcard].dsty = 0.25f + 0.5f * ((f32)j / d_state.players[i].ncards);
				d_state.data[pcard].dstrot = GLM_PI / 2.0f;
				break;
			case D_TOP:
				d_state.data[pcard].dstx = 0.25f + 0.5f * ((f32)j / d_state.players[i].ncards);
				d_state.data[pcard].dsty = 1.0f;
				d_state.data[pcard].dstrot = 0.0f;
				break;
			case D_RIGHT:
				d_state.data[pcard].dstx = 1.0f;
				d_state.data[pcard].dsty = 0.25f + 0.5f * ((f32)j / d_state.players[i].ncards);
				d_state.data[pcard].dstrot = GLM_PI / 2.0f;
				break;
			}
		}
	}
	
	for (usize i = 0; i < d_state.attack.ncards; ++i)
	{
		u8 pcardatk = d_state.attack.pcards[i];
		
		d_state.data[pcardatk].flags &= ~D_COVERUP;
		d_state.data[pcardatk].dstx = 0.4f + 0.45f * ((f32)i / d_state.attack.ncards);
		d_state.data[pcardatk].dsty = 0.42f;
		d_state.data[pcardatk].dstrot = 0.0f;
		
		if (i < d_state.defend.ncards)
		{
			u8 pcarddef = d_state.defend.pcards[i];
			
			d_state.data[pcarddef].flags &= ~D_COVERUP;
			d_state.data[pcarddef].dstx = 0.4f + 0.45f * ((f32)i / d_state.attack.ncards);
			d_state.data[pcarddef].dsty = 0.36f;
			d_state.data[pcarddef].dstrot = 0.0f;
		}
	}
	
	// update card data.
	for (usize i = 0; i < D_NMAXCARDS; ++i)
	{
		d_updatecard(&d_state.data[i]);
	}
	
	if (t_active())
	{
		return;
	}
	
	// update game state.
	switch (d_state.phase)
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
		i32 def = d_nextplayer(d_state.attacker);
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

static void
d_aiattack(d_pcards_t *pc, d_ai_t ai)
{
	if (d_state.attack.ncards)
	{
		switch (ai)
		{
		case D_AGGRESSIVE:
		{
			i32 idx = d_reattack(pc, true);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_MERCIFUL:
		{
			if (d_state.attack.ncards >= 3)
			{
				d_allcovered();
				break;
			}
			
			i32 idx = d_reattack(pc, false);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
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
			
			i32 idx = d_reattack(pc, randint(0, 100) < D_BALANCEDHIATTACK);
			if (idx == -1)
			{
				d_allcovered();
				break;
			}
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
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
			for (usize i = 0; i < pc->ncards; ++i)
			{
				if (d_power(pc->pcards[i]) > power)
				{
					power = d_power(pc->pcards[i]);
					idx = i;
				}
			}
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_MERCIFUL:
		{
			i32 power = 1000;
			usize idx = 0;
			for (usize i = 0; i < pc->ncards; ++i)
			{
				if (d_power(pc->pcards[i]) < power)
				{
					power = d_power(pc->pcards[i]);
					idx = i;
				}
			}
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
			d_setphase(D_DEFEND);
			
			break;
		}
		case D_BALANCED:
		{
			usize idx = randint(0, (pc->ncards + D_BALANCEDMID - 1) / D_BALANCEDMID);
			
			d_addcard(&d_state.attack, pc->pcards[idx]);
			d_rmcard(pc, idx);
			d_randcardsfx();
			
			d_setphase(D_DEFEND);
			
			break;
		}
		}
	}
}

static void
d_aidefend(d_pcards_t *pc)
{
	if (!pc->ncards)
	{
		d_takeall();
		return;
	}
	
	u8 pcardatk = d_state.attack.pcards[d_state.attack.ncards - 1];
	
	i32 idx = -1;
	i32 power = 1000;
	for (i32 i = 0; i < pc->ncards; ++i)
	{
		if (d_powercmp(pc->pcards[i], pcardatk) <= 0)
		{
			continue;
		}
		
		if (d_power(pc->pcards[i]) < power)
		{
			power = d_power(pc->pcards[i]);
			idx = i;
		}
	}
	
	if (idx == -1)
	{
		d_takeall();
		return;
	}
	
	d_addcard(&d_state.defend, pc->pcards[idx]);
	d_rmcard(pc, idx);
	d_randcardsfx();
	
	d_setphase(D_ATTACK);
}

static void
d_playattack(d_pcards_t *pc)
{
	d_state.selidx = d_state.selidx >= pc->ncards ? 0 : d_state.selidx;
	
	if (i_kpressed(o_dyn.kright))
	{
		d_state.selidx = d_state.selidx + 1 >= pc->ncards ? 0 : d_state.selidx + 1;
	}
	
	if (i_kpressed(o_dyn.kleft))
	{
		d_state.selidx = d_state.selidx == 0 ? pc->ncards - 1 : d_state.selidx - 1;
	}
	
	if (d_state.attack.ncards)
	{
		if (pc->ncards && i_kpressed(o_dyn.ksel))
		{
			u8 pcardsel = pc->pcards[d_state.selidx];
			d_card_t const *cardsel = &d_state.data[pcardsel];
			
			for (usize i = 0; i < d_state.attack.ncards; ++i)
			{
				u8 pcardatk = d_state.attack.pcards[i];
				d_card_t const *cardatk = &d_state.data[pcardatk];
				
				if (cardsel->value == cardatk->value)
				{
					d_addcard(&d_state.attack, pcardsel);
					d_rmcard(pc, d_state.selidx);
					d_randcardsfx();
					
					d_state.selidx = -1;
					d_setphase(D_DEFEND);
					
					return;
				}
			}
			
			for (usize i = 0; i < d_state.defend.ncards; ++i)
			{
				u8 pcarddef = d_state.defend.pcards[i];
				d_card_t const *carddef = &d_state.data[pcarddef];
				
				if (cardsel->value == carddef->value)
				{
					d_addcard(&d_state.attack, pcardsel);
					d_rmcard(pc, d_state.selidx);
					d_randcardsfx();
					
					d_state.selidx = -1;
					d_setphase(D_DEFEND);
					
					return;
				}
			}
		}
		else if (!pc->ncards || i_kpressed(o_dyn.kskip))
		{
			d_state.selidx = -1;
			d_allcovered();
		}
	}
	else
	{
		if (i_kpressed(o_dyn.ksel))
		{
			d_addcard(&d_state.attack, pc->pcards[d_state.selidx]);
			d_rmcard(pc, d_state.selidx);
			d_randcardsfx();
			
			d_state.selidx = -1;
			d_setphase(D_DEFEND);
		}
	}
}

static void
d_playdefend(d_pcards_t *pc)
{
	d_state.selidx = d_state.selidx >= pc->ncards ? 0 : d_state.selidx;
	
	if (i_kpressed(o_dyn.kright))
	{
		d_state.selidx = d_state.selidx + 1 >= pc->ncards ? 0 : d_state.selidx + 1;
	}
	
	if (i_kpressed(o_dyn.kleft))
	{
		d_state.selidx = d_state.selidx == 0 ? pc->ncards - 1 : d_state.selidx - 1;
	}
	
	if (pc->ncards && i_kpressed(o_dyn.ksel))
	{
		u8 pcardsel = pc->pcards[d_state.selidx];
		u8 pcardatk = d_state.attack.pcards[d_state.attack.ncards - 1];
		
		if (d_powercmp(pcardsel, pcardatk) > 0)
		{
			d_addcard(&d_state.defend, pcardsel);
			d_rmcard(pc, d_state.selidx);
			d_randcardsfx();
			
			d_state.selidx = -1;
			d_setphase(D_ATTACK);
		}
	}
	else if (!pc->ncards || i_kpressed(o_dyn.kskip))
	{
		d_state.selidx = -1;
		d_takeall();
	}
}

static i32
d_power(u8 pcard)
{
	d_card_t const *card = &d_state.data[pcard];
	
	i32 val = card->value;
	val += 256 * (card->suit == d_state.trumpsuit);
	
	return val;
}

static i32
d_powercmp(u8 plhs, u8 prhs)
{
	d_card_t const *lhs = &d_state.data[plhs];
	d_card_t const *rhs = &d_state.data[prhs];
	
	if (lhs->suit == d_state.trumpsuit && rhs->suit != d_state.trumpsuit)
	{
		return 1;
	}
	else if (lhs->suit != d_state.trumpsuit && rhs->suit == d_state.trumpsuit)
	{
		return -1;
	}
	else if (lhs->suit == rhs->suit)
	{
		return lhs->value - rhs->value;
	}
	else
	{
		// non-comparable cards.
		return 0;
	}
}

static i32
d_nextplayer(i32 from)
{
	// allow chaining of d_nextplayer.
	if (from == -1)
	{
		return -1;
	}
	
	for (i32 i = 1; i <= D_PLAYER_END; ++i)
	{
		u8 player = (from + i) % D_PLAYER_END;
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
	i32 next = d_nextplayer(d_state.attacker);
	if (next == -1)
	{
		return;
	}
	
	d_randcardsfx();
	
	// transfer attack cards to covered pile.
	while (d_state.attack.ncards)
	{
		d_addcard(&d_state.covered, d_state.attack.pcards[0]);
		d_rmcard(&d_state.attack, 0);
	}
	
	// transfer defend cards to covered pile.
	while (d_state.defend.ncards)
	{
		d_addcard(&d_state.covered, d_state.defend.pcards[0]);
		d_rmcard(&d_state.defend, 0);
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
		d_state.playersactive &= ~(1 << next);
	}
	
	// advance attacker.
	d_state.attacker = d_nextplayer(d_state.attacker);
	
	d_setphase(d_activeplayers() <= 1 ? D_FINISH : D_ATTACK);
}

static void
d_takeall(void)
{
	i32 next = d_nextplayer(d_state.attacker);
	if (next == -1)
	{
		return;
	}
	
	d_randcardsfx();
	
	// transfer attack cards to defender.
	while (d_state.attack.ncards)
	{
		d_addcard(&d_state.players[next], d_state.attack.pcards[0]);
		d_rmcard(&d_state.attack, 0);
	}
	
	// transfer defend cards to defender.
	while (d_state.defend.ncards)
	{
		d_addcard(&d_state.players[next], d_state.defend.pcards[0]);
		d_rmcard(&d_state.defend, 0);
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
	d_state.attacker = d_nextplayer(next);
	
	d_setphase(d_activeplayers() <= 1 ? D_FINISH : D_ATTACK);
}

static void
d_drawto(d_pcards_t *pc, usize ncards)
{
	while (pc->ncards < ncards && d_state.draw.ncards)
	{
		d_addcard(pc, d_state.draw.pcards[0]);
		d_rmcard(&d_state.draw, 0);
	}
	
	d_sort(pc);
}

static i32
d_reattack(d_pcards_t *pc, bool hiattack)
{
	if (hiattack)
	{
		for (i32 i = pc->ncards - 1; i >= 0; --i)
		{
			d_card_t const *srccard = &d_state.data[pc->pcards[i]];
			
			for (usize j = 0; j < d_state.attack.ncards; ++j)
			{
				d_card_t const *atkcard = &d_state.data[d_state.attack.pcards[j]];
				if (srccard->value == atkcard->value)
				{
					return i;
				}
			}
			
			for (usize j = 0; j < d_state.defend.ncards; ++j)
			{
				d_card_t const *defcard = &d_state.data[d_state.defend.pcards[j]];
				if (srccard->value == defcard->value)
				{
					return i;
				}
			}
		}
	}
	else
	{
		for (i32 i = 0; i < pc->ncards; ++i)
		{
			d_card_t const *srccard = &d_state.data[pc->pcards[i]];
			
			for (usize j = 0; j < d_state.attack.ncards; ++j)
			{
				d_card_t const *atkcard = &d_state.data[d_state.defend.pcards[j]];
				if (srccard->value == atkcard->value)
				{
					return i;
				}
			}
			
			for (usize j = 0; j < d_state.defend.ncards; ++j)
			{
				d_card_t const *defcard = &d_state.data[d_state.defend.pcards[j]];
				if (srccard->value == defcard->value)
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

static void
d_updatecard(d_card_t *card)
{
	card->x = glm_lerp(card->x, card->dstx, O_CARDSPEED);
	card->y = glm_lerp(card->y, card->dsty, O_CARDSPEED);
	card->rot = interpangle(card->rot, card->dstrot, O_CARDSPEED);
}

static void
d_shuffle(d_pcards_t *pc)
{
	for (i32 i = 0; i < pc->ncards; ++i)
	{
		usize other = randint(i, pc->ncards);
		u8 tmp = pc->pcards[other];
		pc->pcards[other] = pc->pcards[i];
		pc->pcards[i] = tmp;
	}
}

static void
d_addcard(d_pcards_t *pc, u8 pcard)
{
	if (pc->ncards < D_NMAXCARDS)
	{
		pc->pcards[pc->ncards++] = pcard;
	}
}

static void
d_rmcard(d_pcards_t *pc, usize idx)
{
	hmemmove(&pc->pcards[idx], &pc->pcards[idx + 1], --pc->ncards - idx);
}

static void
d_sort(d_pcards_t *pc)
{
	for (usize i = 0; i < pc->ncards; ++i)
	{
		for (usize j = 1; j < pc->ncards; ++j)
		{
			if (d_power(pc->pcards[j]) < d_power(pc->pcards[j - 1]))
			{
				u8 tmp = pc->pcards[j];
				pc->pcards[j] = pc->pcards[j - 1];
				pc->pcards[j - 1] = tmp;
			}
		}
	}
}

static void
d_rendercard(d_card_t const *card)
{
	if ((card->flags & D_SHOW) == 0)
	{
		return;
	}
	
	i32 rw, rh;
	r_renderbounds(&rw, &rh);
	
	i32 absx = card->x * rw - O_CARDWIDTH / 2;
	i32 absy = card->y * rh - O_CARDHEIGHT / 2;
	
	u8 tex = R_CBACK;
	if ((card->flags & D_COVERUP) == 0)
	{
		switch (card->suit)
		{
		case D_SPADES:
			tex = R_CS6 + card->value - 1;
			break;
		case D_DIAMONDS:
			tex = R_CD6 + card->value - 1;
			break;
		case D_CLUBS:
			tex = R_CC6 + card->value - 1;
			break;
		case D_HEARTS:
			tex = R_CH6 + card->value - 1;
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
		card->rot
	);
	r_renderrect(tex, absx, absy, O_CARDWIDTH, O_CARDHEIGHT, card->rot);
}

static void
d_randcardsfx(void)
{
	s_sfx_t sfx = randint(S_CARD0, S_CARD8 + 1);
	s_playsfx(sfx);
}
