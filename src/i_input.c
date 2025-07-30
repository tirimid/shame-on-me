// SPDX-License-Identifier: GPL-3.0-or-later

// assume that 1024 represents a big enough number of keystates.
static u8 i_kdownstates[1024 / 8];
static u8 i_kpressstates[1024 / 8], i_kreleasestates[1024 / 8];

// SDL2 supports 5 mouse buttons.
static u8 i_mdownstates[5];
static u8 i_mpressstates[5], i_mreleasestates[5];

void
i_handle(SDL_Event const *e)
{
	switch (e->type)
	{
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	{
		if (e->key.repeat)
		{
			return;
		}
		
		bool state = e->type == SDL_KEYDOWN;
		
		SDL_Keycode k = e->key.keysym.sym;
		if (k & 1 << 30)
		{
			k &= ~(1 << 30);
			k += 128;
		}
		
		usize byte = k / 8, bit = k % 8;
		
		if (state)
		{
			i_kdownstates[byte] |= 1 << bit;
			i_kpressstates[byte] |= 1 << bit;
		}
		else
		{
			i_kdownstates[byte] &= ~(1 << bit);
			i_kreleasestates[byte] |= 1 << bit;
		}
		
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	{
		bool state = e->type == SDL_MOUSEBUTTONDOWN;
		
		if (state)
		{
			i_mdownstates[e->button.button] = true;
			i_mpressstates[e->button.button] = true;
		}
		else
		{
			i_mdownstates[e->button.button] = false;
			i_mreleasestates[e->button.button] = true;
		}
		
		break;
	}
	default:
		break;
	}
}

void
i_prepare(void)
{
	memset(i_kpressstates, 0, sizeof(i_kpressstates));
	memset(i_kreleasestates, 0, sizeof(i_kreleasestates));
	memset(i_mpressstates, 0, sizeof(i_mpressstates));
	memset(i_mreleasestates, 0, sizeof(i_mreleasestates));
}

bool
i_kdown(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return i_kdownstates[byte] & 1 << bit;
}

bool
i_kpressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return i_kpressstates[byte] & 1 << bit;
}

bool
i_kreleased(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return i_kreleasestates[byte] & 1 << bit;
}

void
i_mpos(OUT i32 *x, OUT i32 *y)
{
	if (SDL_GetMouseFocus() != r_wnd)
	{
		*x = 0;
		*y = 0;
		return;
	}
	
	SDL_GetMouseState(x, y);
}

bool
i_mdown(i32 btn)
{
	return i_mdownstates[btn];
}

bool
i_mpressed(i32 btn)
{
	return i_mpressstates[btn];
}

bool
i_mreleased(i32 btn)
{
	return i_mreleasestates[btn];
}
