// SPDX-License-Identifier: GPL-3.0-or-later

// assume that 1024 represents a big enough number of keystates.
static u8 i_kdownstates[1024 / 8];
static u8 i_kpressstates[1024 / 8], i_kreleasestates[1024 / 8];

void
i_setkstate(SDL_Event const *e, i_input_t input)
{
	if (e->key.repeat)
	{
		return;
	}
	
	SDL_Keycode k = e->key.keysym.sym;
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize byte = k / 8, bit = k % 8;
	
	if (input)
	{
		i_kdownstates[byte] |= 1 << bit;
		i_kpressstates[byte] |= 1 << bit;
	}
	else
	{
		i_kdownstates[byte] &= ~(1 << bit);
		i_kreleasestates[byte] &= ~(1 << bit);
	}
}

void
i_prepare(void)
{
	memset(i_kdownstates, 0, sizeof(i_kdownstates));
	memset(i_kpressstates, 0, sizeof(i_kpressstates));
	memset(i_kreleasestates, 0, sizeof(i_kreleasestates));
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
