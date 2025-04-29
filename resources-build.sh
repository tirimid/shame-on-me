#!/bin/bash

INCLUDE="-I."
DEFINES=""
WARNING="-Wall -Wextra -Wshadow"
LIBRARIES=""
CFLAGS="-std=c99 -pedantic"

CC=gcc
CFLAGS_FULL="$INCLUDE $DEFINES $WARNINGS $CFLAGS $LIBRARIES"

echo "[$0] resources: make images" >&2
make -B -s -C img > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make images!" >&2
	exit 1
fi

echo "[$0] resources: make models" >&2
make -B -s -C model > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make models!" >&2
	exit 1
fi

echo "[$0] resources: make shaders" >&2
make -B -s -C shader > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make shaders!" >&2
	exit 1
fi

echo "[$0] resources: make fonts" >&2
make -B -s -C font > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make fonts!" >&2
	exit 1
fi

echo "[$0] resources: make sounds" >&2
make -B -s -C sound > /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to make sounds!" >&2
	exit 1
fi

echo "[$0] resources: compilation" >&2
$CC -o resources.o -c src/resources.c $CFLAGS_FULL
if [ $? -ne 0 ]
then
	echo "[$0] resources: failed to compile!" >&2
	exit 1
fi

echo "[$0] resources: finished successfully" >&2
