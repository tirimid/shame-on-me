// SPDX-License-Identifier: GPL-3.0-or-later

#define S_INCOGG(name) \
	{ \
		.data = name##_ogg, \
		.size = &name##_ogg_len \
	}

#define S_FREQ 44100
#define S_CHUNKSIZE 256

typedef struct s_sound
{
	u8 const *data;
	u32 const *size;
	Mix_Chunk *chunk;
} s_sound;

static s_sound s_sfxsounds[S_SFX_END] =
{
	S_INCOGG(speak_arkady),
	S_INCOGG(speak_gerasim),
	S_INCOGG(speak_matthew),
	S_INCOGG(speak_peter),
	S_INCOGG(card0),
	S_INCOGG(card1),
	S_INCOGG(card2),
	S_INCOGG(card3),
	S_INCOGG(card4),
	S_INCOGG(card5),
	S_INCOGG(card6),
	S_INCOGG(card7),
	S_INCOGG(card8),
	S_INCOGG(card_shuffle)
};

i32
s_init(void)
{
	if (Mix_OpenAudio(S_FREQ, MIX_DEFAULT_FORMAT, 2, S_CHUNKSIZE))
	{
		showerr("sound: failed to open audio device - %s!", Mix_GetError());
		return 1;
	}
	
	atexit(s_quit);
	
	// allocate SFX sound references.
	for (usize i = 0; i < S_SFX_END; ++i)
	{
		SDL_RWops *rwops = SDL_RWFromConstMem(s_sfxsounds[i].data, *s_sfxsounds[i].size);
		if (!rwops)
		{
			showerr("sound: failed to create RWops - %s!", SDL_GetError());
			return 1;
		}
		
		s_sfxsounds[i].chunk = Mix_LoadWAV_RW(rwops, 1);
		if (!s_sfxsounds[i].chunk)
		{
			showerr("sound: failed to create Mix_Chunk - %s!", Mix_GetError());
			return 1;
		}
	}
	
	// set mix parameters.
	Mix_AllocateChannels(16);
	s_sfxvolume(O_VOLUME);
	
	return 0;
}

void
s_quit(void)
{
	// free SFX sound references.
	for (usize i = 0; i < S_SFX_END; ++i)
	{
		Mix_FreeChunk(s_sfxsounds[i].chunk);
	}
	
	Mix_CloseAudio();
}

void
s_sfxvolume(f32 vol)
{
	vol = vol < 0.0f ? 0.0f : vol;
	vol = vol > 1.0f ? 1.0f : vol;
	Mix_Volume(-1, vol * MIX_MAX_VOLUME);
}

void
s_playsfx(s_sfx id)
{
	Mix_PlayChannel(-1, s_sfxsounds[id].chunk, 0);
}
