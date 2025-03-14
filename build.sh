#!/bin/bash

CC=gcc
CFLAGS="-I. -Iinclude -Wall -Wextra -std=c99 -pedantic -O3 $(pkg-config --cflags --libs sdl2 gl SDL2_mixer SDL2_ttf SDL2_image cglm glew) -lm"

make -s -C img > /dev/null
make -s -C model > /dev/null
make -s -C shader > /dev/null
make -s -C font > /dev/null

$CC -o shame-on-me src/main.c $CFLAGS
if [ $? -ne 0 ]
then
	echo "err: build failed!" >&2
	exit 1
fi
