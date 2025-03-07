#!/bin/bash

CC=gcc
CFLAGS="-I. -Iinclude -Idep -Wall -std=c99 -pedantic $(pkg-config --cflags --libs sdl2 gl SDL2_mixer SDL2_ttf SDL2_image cglm glew)"

$CC -o shame-on-me src/main.c $CFLAGS
if [ $? -ne 0 ]
then
	echo "err: build failed!" >&2
	exit 1
fi
