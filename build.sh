#!/bin/bash

CC=gcc
CFLAGS="-I. -Isrc -Wall -Wextra -std=c99 -pedantic -O3 $(pkg-config --cflags --libs sdl2 gl SDL2_mixer SDL2_ttf SDL2_image cglm glew) -lm"

echo "[$0] build: resources" >&2
make -s -C img > /dev/null
make -s -C model > /dev/null
make -s -C shader > /dev/null
make -s -C font > /dev/null

echo "[$0] build: compilation" >&2
$CC -o shame-on-me src/main.c $CFLAGS
if [ $? -ne 0 ]
then
	echo "[$0] build: failed to compile!" >&2
	exit 1
fi

echo "[$0] build: finished successfully" >&2
