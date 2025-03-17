static u64 TickStartTime;

void
ShowError(char const *Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	
	char Msg[O_MAX_LOG_LEN];
	vsnprintf(Msg, O_MAX_LOG_LEN, Fmt, Args);
	
	// stderr is backup so that error can still be processed in case a message
	// box can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, O_ERR_WND_TITLE, Msg, NULL))
		fprintf(stderr, "err: %s\n", Msg);
	
	va_end(Args);
}

u64
GetUnixTimeUs(void)
{
	struct timeval Tv;
	gettimeofday(&Tv, NULL);
	return (u64)Tv.tv_sec * 1000000 + (u64)Tv.tv_usec;
}

u64
GetUnixTimeMs(void)
{
	return GetUnixTimeUs() / 1000;
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
	i64 TickTimeLeft = O_TICK_MS - TickEndTime + TickStartTime;
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

void
MakeXformMat(vec3 Pos, vec3 Rot, vec3 Scale, mat4 Out)
{
	mat4 TransMat, RotMat, ScaleMat;
	glm_translate_make(TransMat, Pos);
	glm_euler(Rot, RotMat);
	glm_scale_make(ScaleMat, Scale);
	
	glm_mat4_identity(Out);
	glm_mat4_mul(Out, TransMat, Out);
	glm_mat4_mul(Out, RotMat, Out);
	glm_mat4_mul(Out, ScaleMat, Out);
}

void
MakeNormalMat(mat4 Xform, mat3 Out)
{
	mat4 Tmp;
	glm_mat4_inv_fast(Xform, Tmp);
	glm_mat4_transpose(Tmp);
	glm_mat4_pick3(Tmp, Out);
}

void
BeginMicroTimer(u64 *OutTimer)
{
	*OutTimer = GetUnixTimeUs();
}

void
EndMicroTimer(u64 TimerStart, char const *Name)
{
	u64 Delta = GetUnixTimeUs() - TimerStart;
	fprintf(stderr, "[profile] %s: %lu\n", Name, Delta);
}
