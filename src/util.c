// SPDX-License-Identifier: GPL-3.0-or-later

static u64 u_tickstart;

void
showerr(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[O_MAXLOGLEN];
	vsnprintf(msg, O_MAXLOGLEN, fmt, args);
	
	// stderr is backup so that error can still be processed in case a messagebox
	// can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, O_ERRWNDTITLE, msg, NULL))
	{
		fprintf(stderr, "err: %s\n", msg);
	}
	
	va_end(args);
}

u64
unixus(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (u64)tv.tv_sec * 1000000 + (u64)tv.tv_usec;
}

void
begintick(void)
{
	u_tickstart = unixus();
}

void
endtick(void)
{
	u64 tickend = unixus();
	i64 timeleft = O_TICKUS - tickend + u_tickstart;
	timeleft *= (timeleft > 0);
	
	struct timespec ts =
	{
		.tv_nsec = 1000 * timeleft
	};
	nanosleep(&ts, NULL);
}

f32
interpangle(f32 a, f32 b, f32 t)
{
	return a + shortestangle(a, b) * t;
}

f32
shortestangle(f32 a, f32 b)
{
	f32 d = fmod(b - a, 2.0f * GLM_PI);
	f32 shortest = fmod(2.0f * d, 2.0f * GLM_PI) - d;
	return shortest;
}

void
makexform(vec3 pos, vec3 rot, vec3 scale, OUT mat4 out)
{
	mat4 transm, rotm, scalem;
	glm_translate_make(transm, pos);
	glm_euler(rot, rotm);
	glm_scale_make(scalem, scale);
	
	glm_mat4_identity(out);
	glm_mat4_mul(out, transm, out);
	glm_mat4_mul(out, rotm, out);
	glm_mat4_mul(out, scalem, out);
}

void
makenorm(mat4 xform, OUT mat3 out)
{
	mat4 tmp;
	glm_mat4_inv_fast(xform, tmp);
	glm_mat4_transpose(tmp);
	glm_mat4_pick3(tmp, out);
}

void
begintimer(OUT u64 *timer)
{
	*timer = unixus();
}

void
endtimer(u64 timer, char const *name)
{
	u64 d = unixus() - timer;
	fprintf(stderr, "[profile] %s: %llu\n", name, (unsigned long long)d);
}

i32
randint(i32 lb, i32 ub)
{
	if (ub - lb)
	{
		return rand() % (ub - lb) + lb;
	}
	else
	{
		return 0;
	}
}

f32
randfloat(f32 lb, f32 ub)
{
	i32 iub = 100.0f * (ub - lb);
	f32 rand = randint(0, iub) / 100.0f;
	return rand + lb;
}

void *
hmemcpy(void *dst, void const *src, usize n)
{
	if (!n)
	{
		return dst;
	}
	
	return memcpy(dst, src, n);
}

void *
hmemmove(void *dst, void const *src, usize n)
{
	if (!n)
	{
		return dst;
	}
	
	return memmove(dst, src, n);
}
