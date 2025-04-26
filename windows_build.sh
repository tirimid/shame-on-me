#!/bin/bash

INCLUDE="-I. -Isrc"
DEFINES=""
WARNINGS="-Wall -Wextra -Wshadow"
LIBRARIES="$(pkg-config --cflags --libs sdl2 SDL2_mixer SDL2_ttf SDL2_image cglm) -lglew32 -lglu32 -lopengl32 -lm"
CFLAGS="-std=c99 -pedantic -O3"

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

# resource build not supported on windows.

echo "[$0] build: compilation" >&2
$CC -o shame-on-me src/main.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2
