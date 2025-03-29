#!/bin/bash

INCLUDE="-I. -Isrc"
DEFINES="-DPROFILE"
#DEFINES=""
WARNINGS="-Wall -Wextra"
LIBRARIES="$(pkg-config --cflags --libs sdl2 gl SDL2_mixer SDL2_ttf SDL2_image cglm glew) -lm"
CFLAGS="-std=c99 -pedantic -march=native -O3 -g"

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] build: resources" >&2
make -s -C img > /dev/null
make -s -C model > /dev/null
make -s -C shader > /dev/null
make -s -C font > /dev/null

echo "[$0] build: compilation" >&2
$CC -o shame-on-me src/main.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2
