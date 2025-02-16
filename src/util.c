#include "util.h"

#include <stdarg.h>
#include <stdio.h>

#include <SDL.h>

#define ERR_TITLE "FMOSOM - Error"
#define MAX_LOG_LEN 512

void
LogErr(char const *Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);
	
	char Msg[MAX_LOG_LEN];
	vsnprintf(Msg, MAX_LOG_LEN, Fmt, Args);
	
	// stderr is backup so that error can still be processed in case a message
	// box can't be opened.
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, ERR_TITLE, Msg, NULL))
		fprintf(stderr, "err: %s\n", Msg);
	
	va_end(Args);
}
