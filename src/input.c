// assume that 1024 represents a big enough number of keystates.
static u8 I_KDownStates[1024 / 8];
static u8 I_KPressStates[1024 / 8], I_KReleaseStates[1024 / 8];

void
I_SetKeybdState(SDL_Event const *e, I_InputType IT)
{
	SDL_Keycode k = e->key.keysym.sym;
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	
	usize Byte = k / 8, Bit = k % 8;
	
	if (IT)
	{
		I_KDownStates[Byte] |= 1 << Bit;
		I_KPressStates[Byte] |= 1 << Bit;
	}
	else
	{
		I_KDownStates[Byte] &= ~(1 << Bit);
		I_KReleaseStates[Byte] &= ~(1 << Bit);
	}
}

void
I_Prepare(void)
{
	memset(I_KDownStates, 0, sizeof(I_KDownStates));
	memset(I_KPressStates, 0, sizeof(I_KPressStates));
	memset(I_KReleaseStates, 0, sizeof(I_KReleaseStates));
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
	return I_KDownStates[Byte] & 1 << Bit;
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
	return I_KPressStates[Byte] & 1 << Bit;
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
	return I_KReleaseStates[Byte] & 1 << Bit;
}
