static u64 TickStartTime;

void
ShowError(char const *Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	
	char Msg[CF_MAX_LOG_LEN];
	vsnprintf(Msg, CF_MAX_LOG_LEN, Fmt, Args);
	
	// stderr is backup so that error can still be processed in case a message
	// box can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, CF_ERR_WND_TITLE, Msg, NULL))
		fprintf(stderr, "err: %s\n", Msg);
	
	va_end(Args);
}

u64
GetUnixTimeMs(void)
{
	struct timeval Tv;
	gettimeofday(&Tv, NULL);
	return (u64)Tv.tv_sec * 1000 + (u64)Tv.tv_usec / 1000;
}

void
BeginTick(void)
{
	TickStartTime = GetUnixTimeMs();
}

void
EndTick(void)
{
	u64 TickEndTime = GetUnixTimeMs();
	i64 TickTimeLeft = CF_TICK_MS - TickEndTime + TickStartTime;
	SDL_Delay(TickTimeLeft * (TickTimeLeft > 0));
}

f32
InterpolateAngle(f32 a, f32 b, f32 t)
{
	return a + ShortestAngle(a, b) * t;
}

f32
ShortestAngle(f32 a, f32 b)
{
	f32 d = fmod(b - a, 2.0f * GLM_PI);
	f32 Shortest = fmod(2.0f * d, 2.0f * GLM_PI) - d;
	return Shortest;
}
