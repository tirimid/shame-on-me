// SPDX-License-Identifier: GPL-3.0-or-later

#define S_INCOGG(name) \
	{ \
		.data = name##_ogg, \
		.size = &name##_ogg_len \
	}

#define S_FREQ 44100
#define S_CHUNKSIZE 256

typedef enum s_musicstate
{
	S_STOPPED = 0,
	S_PLAYING,
	S_STARTING,
	S_STOPPING
} s_musicstate_t;

typedef struct s_sound
{
	u8 const *data;
	u32 const *size;
	union
	{
		Mix_Chunk *chunk;
		Mix_Music *music;
	} mixdata;
} s_sound_t;

static s_sound_t s_sfxsounds[S_SFX_END] =
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
	S_INCOGG(card_shuffle),
	S_INCOGG(explode),
	S_INCOGG(explode_muffled),
	S_INCOGG(knock)
};

static s_sound_t s_musicsounds[S_MUSIC_END] =
{
	S_INCOGG(bg_ominous)
};

static s_musicstate_t s_musicstate = S_STOPPED;
static s_music_t s_nextmusic;
static f32 s_musicfade = 0.0f;

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
		
		s_sfxsounds[i].mixdata.chunk = Mix_LoadWAV_RW(rwops, 1);
		if (!s_sfxsounds[i].mixdata.chunk)
		{
			showerr("sound: failed to create Mix_Chunk - %s!", Mix_GetError());
			return 1;
		}
	}
	
	// allocate music sound references.
	for (usize i = 0; i < S_MUSIC_END; ++i)
	{
		SDL_RWops *rwops = SDL_RWFromConstMem(s_musicsounds[i].data, *s_musicsounds[i].size);
		if (!rwops)
		{
			showerr("sound: failed to create RWops - %s!", SDL_GetError());
			return 1;
		}
		
		s_musicsounds[i].mixdata.music = Mix_LoadMUS_RW(rwops, 1);
		if (!s_musicsounds[i].mixdata.music)
		{
			showerr("sound: failed to create Mix_Music - %s!", Mix_GetError());
			return 1;
		}
	}
	
	// set mix parameters.
	Mix_AllocateChannels(16);
	s_sfxvolume(o_dyn.sfxvolume);
	
	return 0;
}

void
s_quit(void)
{
	// free music sound references.
	for (usize i = 0; i < S_MUSIC_END; ++i)
	{
		Mix_FreeMusic(s_musicsounds[i].mixdata.music);
	}
	
	// free SFX sound references.
	for (usize i = 0; i < S_SFX_END; ++i)
	{
		Mix_FreeChunk(s_sfxsounds[i].mixdata.chunk);
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
s_playsfx(s_sfx_t id)
{
	Mix_PlayChannel(-1, s_sfxsounds[id].mixdata.chunk, 0);
}

void
s_playmusic(s_music_t id)
{
	s_nextmusic = id;
	s_musicstate = S_STOPPING;
}

void
s_update(void)
{
	// update music.
	switch (s_musicstate)
	{
	case S_STOPPED:
		if (s_nextmusic != S_MUSIC_END)
		{
			Mix_PlayMusic(s_musicsounds[s_nextmusic].mixdata.music, -1);
			s_musicstate = S_STARTING;
		}
		break;
	case S_STARTING:
		s_musicfade += O_MUSICFADE;
		if (s_musicfade >= 1.0f)
		{
			s_musicfade = 1.0f;
			s_musicstate = S_PLAYING;
		}
		break;
	case S_STOPPING:
		s_musicfade -= O_MUSICFADE;
		if (s_musicfade <= 0.0f)
		{
			s_musicfade = 0.0f;
			s_musicstate = S_STOPPED;
			if (Mix_PlayingMusic())
			{
				Mix_HaltMusic();
			}
		}
		break;
	default:
		break;
	}
	
	Mix_VolumeMusic(s_musicfade * o_dyn.musicvolume * MIX_MAX_VOLUME);
}
