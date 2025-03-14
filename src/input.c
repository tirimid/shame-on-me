// assume that 1024 represents a big enough number of keystates.
static u8 KDownStates[1024 / 8];
static u8 KPressStates[1024 / 8], KReleaseStates[1024 / 8];

void
I_SetKeybdState(SDL_Event const *e, enum InputType It)
{
	SDL_Keycode k = e->key.keysym.sym;
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize Byte = k / 8, Bit = k % 8;
	
	if (It)
	{
		KDownStates[Byte] |= 1 << Bit;
		KPressStates[Byte] |= 1 << Bit;
	}
	else
	{
		KDownStates[Byte] &= ~(1 << Bit);
		KReleaseStates[Byte] &= ~(1 << Bit);
	}
}

void
I_Prepare(void)
{
	memset(KDownStates, 0, sizeof(KDownStates));
	memset(KPressStates, 0, sizeof(KPressStates));
	memset(KReleaseStates, 0, sizeof(KReleaseStates));
}

bool
I_KeyDown(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize Byte = k / 8, Bit = k % 8;
	return KDownStates[Byte] & 1 << Bit;
}

bool
I_KeyPressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize Byte = k / 8, Bit = k % 8;
	return KPressStates[Byte] & 1 << Bit;
}

bool
I_KeyReleased(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize Byte = k / 8, Bit = k % 8;
	return KReleaseStates[Byte] & 1 << Bit;
}
