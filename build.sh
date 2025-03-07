#!/bin/bash

CC=gcc
LD=gcc
CFLAGS="-I. -Iinclude -Wall -std=c99 -pedantic $(pkg-config --cflags sdl2 gl SDL2_mixer SDL2_ttf cglm glew)"
LDFLAGS="$(pkg-config --cflags --libs sdl2 gl SDL2_mixer SDL2_ttf cglm glew)"

mkdir -p lib

if [ $# -ne 1 ] || [ "$1" == "data" ]
then
	echo "build > data"
	$CC $CFLAGS -o lib/data.o -c src/data.c
	if [ $? -ne 0 ]
	then
		echo "err: build failed on data!" >&2
		exit 1
	fi
fi

if [ $# -ne 1 ] || [ "$1" == "main" ]
then
	echo "build > main"
	$CC $CFLAGS -o lib/main.o -c src/main.c
	if [ $? -ne 0 ]
	then
		echo "err: build failed on main!" >&2
		exit 1
	fi
fi

echo "build > link"
$LD -o fool-me-once-shame-on-me lib/data.o lib/main.o $LDFLAGS
if [ $? -ne 0 ]
then
	echo "err: build failed on link!" >&2
	exit 1
fi
