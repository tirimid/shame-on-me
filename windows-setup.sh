#!/bin/bash

# fetch DLL releases.
mkdir dll-fetch
cd dll-fetch

echo "[$0] setup: fetch SDL2" >&2
wget https://github.com/libsdl-org/SDL/releases/download/release-2.30.1/SDL2-2.30.1-win32-x64.zip 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] setup: failed to fetch!" >&2
	exit 1
fi

echo "[$0] setup: fetch SDL2 mixer" >&2
wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-2.8.0-win32-x64.zip 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] setup: failed to fetch!" >&2
	exit 1
fi

echo "[$0] setup: fetch SDL2 image" >&2
wget https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2-win32-x64.zip 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] setup: failed to fetch!" >&2
	exit 1
fi

echo "[$0] setup: fetch SDL2 TTF" >&2
wget https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-2.24.0-win32-x64.zip 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] setup: failed to fetch!" >&2
	exit 1
fi

echo "[$0] setup: fetch GLEW" >&2
wget https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip 2> /dev/null
if [ $? -ne 0 ]
then
	echo "[$0] setup: failed to fetch!" >&2
	exit 1
fi

echo "[$0] setup: extraction" >&2
unzip -o SDL2-2.30.1-win32-x64.zip > /dev/null
unzip -o SDL2_mixer-2.8.0-win32-x64.zip > /dev/null
unzip -o SDL2_image-2.8.2-win32-x64.zip > /dev/null
unzip -o SDL2_ttf-2.24.0-win32-x64.zip > /dev/null
unzip -o glew-2.2.0-win32.zip > /dev/null
cd ..

echo "[$0] setup: copying fetched DLLs" >&2
cp dll-fetch/SDL2.dll .
cp dll-fetch/SDL2_image.dll .
cp dll-fetch/SDL2_mixer.dll .
cp dll-fetch/SDL2_ttf.dll .
cp dll-fetch/glew-2.2.0/bin/Release/x64/glew32.dll .

echo "[$0] setup: copying system DLLs" >&2
cp /c/Windows/System32/opengl32.dll .
cp /usr/bin/msys-2.0.dll .

# clean up.
rm -rf dll-fetch

echo "[$0] setup: finished successfully" >&2
