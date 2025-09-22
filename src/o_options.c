// SPDX-License-Identifier: GPL-3.0-or-later

#define O_DYNBUFSIZE 128
#define O_DYNSCANFMT "%127s = %127[^\r\n]"

o_dyn_t o_dyn;

static i32 o_getraw(FILE *fp, char const *key, OUT char buf[]);
static i32 o_getkeycode(FILE *fp, char const *key, OUT SDL_Keycode *k);
static i32 o_getfloat(FILE *fp, char const *key, OUT f64 *f);
static i32 o_getint(FILE *fp, char const *key, OUT i64 *i);
static i32 o_getbool(FILE *fp, char const *key, OUT bool *b);

void
o_dyndefault(void)
{
	o_dyn = (o_dyn_t)
	{
		.wndw = 800,
		.wndh = 600,
		.ksel = SDLK_c,
		.kskip = SDLK_z,
		.kright = SDLK_RIGHT,
		.kleft = SDLK_LEFT,
		.sfxvolume = 0.4f,
		.musicvolume = 0.3f,
		.pixelation = O_HIGHPIXELATION,
		.fullscreen = true
	};
}

i32
o_dynread(void)
{
	FILE *fp = fopen(O_DYNFILE, "rb");
	if (!fp)
	{
		// do not display error, instead the program will just try to silently
		// create a fresh configuration file.
		return 1;
	}
	
	if (o_getint(fp, "wndw", &o_dyn.wndw)
		|| o_getint(fp, "wndh", &o_dyn.wndh)
		|| o_getkeycode(fp, "ksel", &o_dyn.ksel)
		|| o_getkeycode(fp, "kskip", &o_dyn.kskip)
		|| o_getkeycode(fp, "kright", &o_dyn.kright)
		|| o_getkeycode(fp, "kleft", &o_dyn.kleft)
		|| o_getfloat(fp, "sfxvolume", &o_dyn.sfxvolume)
		|| o_getfloat(fp, "musicvolume", &o_dyn.musicvolume)
		|| o_getfloat(fp, "pixelation", &o_dyn.pixelation)
		|| o_getbool(fp, "fullscreen", &o_dyn.fullscreen))
	{
		fclose(fp);
		return 1;
	}
	
	fclose(fp);
	
	if (o_dyn.wndw <= 0)
	{
		showerr("options: value for wndw is invalid - %lld!", (long long)o_dyn.wndw);
		return 1;
	}
	
	if (o_dyn.wndh <= 0)
	{
		showerr("options: value for wndh is invalid - %lld!", (long long)o_dyn.wndh);
		return 1;
	}
	
	if (o_dyn.sfxvolume > 1.0f)
	{
		showerr("options: value for sfxvolume is invalid - %f!", o_dyn.sfxvolume);
		return 1;
	}
	
	if (o_dyn.musicvolume > 1.0f)
	{
		showerr("options: value for musicvolume is invalid - %f!", o_dyn.sfxvolume);
		return 1;
	}
	
	if (o_dyn.pixelation < 1.0f)
	{
		showerr("options: value for pixelation is invalid - %f!", o_dyn.pixelation);
		return 1;
	}
	
	return 0;
}

i32
o_dynwrite(void)
{
	FILE *fp = fopen(O_DYNFILE, "wb");
	if (!fp)
	{
		showerr("options: could not open configuration for writing!");
		return 1;
	}
	
	char kselname[32], kskipname[32], krightname[32], kleftname[32];
	sprintf(kselname, "%s", SDL_GetKeyName(o_dyn.ksel));
	sprintf(kskipname, "%s", SDL_GetKeyName(o_dyn.kskip));
	sprintf(krightname, "%s", SDL_GetKeyName(o_dyn.kright));
	sprintf(kleftname, "%s", SDL_GetKeyName(o_dyn.kleft));
	
	fprintf(
		fp,
		"wndw = %lld\n"
		"wndh = %lld\n"
		"ksel = %s\n"
		"kskip = %s\n"
		"kright = %s\n"
		"kleft = %s\n"
		"sfxvolume = %f\n"
		"musicvolume = %f\n"
		"pixelation = %f\n"
		"fullscreen = %s\n",
		(long long)o_dyn.wndw,
		(long long)o_dyn.wndh,
		kselname,
		kskipname,
		krightname,
		kleftname,
		o_dyn.sfxvolume,
		o_dyn.musicvolume,
		o_dyn.pixelation,
		o_dyn.fullscreen ? "true" : "false"
	);
	
	fclose(fp);
	return 0;
}

static i32
o_getraw(FILE *fp, char const *key, OUT char buf[])
{
	fseek(fp, 0, SEEK_SET);
	
	for (usize line = 0; !feof(fp) && !ferror(fp); ++line)
	{
		i32 ch;
		while (ch = fgetc(fp), ch != EOF && isspace(ch))
		{
		}
		
		if (ch == '#')
		{
			while (ch = fgetc(fp), ch != EOF && ch != '\n')
			{
			}
		}
		
		if (ch == '\n' || feof(fp))
		{
			continue;
		}
		
		fseek(fp, -1, SEEK_CUR);
		char keybuf[O_DYNBUFSIZE] = {0};
		if (fscanf(fp, O_DYNSCANFMT, keybuf, buf) != 2)
		{
			showerr("options: error on line %zu of configuration!", line);
			return 1;
		}
		
		if (!strcmp(buf, "NONE"))
		{
			buf[0] = 0;
		}
		
		if (!strcmp(keybuf, key))
		{
			return 0;
		}
	}
	
	showerr("options: did not find key %s in configuration!", key);
	return 1;
}

static i32
o_getkeycode(FILE *fp, char const *key, OUT SDL_Keycode *k)
{
	char buf[O_DYNBUFSIZE] = {0};
	if (o_getraw(fp, key, buf))
	{
		return 1;
	}
	
	*k = SDL_GetKeyFromName(buf);
	if (*k == SDLK_UNKNOWN)
	{
		showerr("options: unknown keycode for key %s - %s!", key, buf);
		return 1;
	}
	
	return 0;
}

static i32
o_getfloat(FILE *fp, char const *key, OUT f64 *f)
{
	char buf[O_DYNBUFSIZE] = {0};
	if (o_getraw(fp, key, buf))
	{
		return 1;
	}
	
	errno = 0;
	*f = strtof(buf, NULL);
	if (errno)
	{
		showerr("options: invalid floating point value for key %s - %s!", key, buf);
		return 1;
	}
	
	return 0;
}

static i32
o_getint(FILE *fp, char const *key, OUT i64 *i)
{
	char buf[O_DYNBUFSIZE] = {0};
	if (o_getraw(fp, key, buf))
	{
		return 1;
	}
	
	errno = 0;
	*i = (i64)strtoll(buf, NULL, 0);
	if (errno)
	{
		showerr("options: invalid integer value for key %s - %s!", key, buf);
		return 1;
	}
	
	return 0;
}

static i32
o_getbool(FILE *fp, char const *key, OUT bool *b)
{
	char buf[O_DYNBUFSIZE] = {0};
	if (o_getraw(fp, key, buf))
	{
		return 1;
	}
	
	if (!strcmp(buf, "true"))
	{
		*b = true;
		return 0;
	}
	else if (!strcmp(buf, "false"))
	{
		*b = false;
		return 0;
	}
	else
	{
		showerr("options: invalid boolean value for key %s - %s!", key, buf);
		return 1;
	}
}
