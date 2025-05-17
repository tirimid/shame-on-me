# shame-on-me

## Introduction

Shame on Me is a mixed 3D/2D game where an apocalyptic situation has brought you
together with your friends. Upon arriving to the house, you find out that their
ambitions in inviting you were not all innocent. As you play a game of cards,
the situation escalates.

## Dependencies

* Make (for asset build, optional)
* xxd (for asset build, optional)
* SDL2
* SDL2 image
* SDL2 mixer
* SDL2 TTF
* MSYS2/MinGW64 (for windows build)
* wget (for windows build setup)
* unzip (for windows build setup)

## Management

* If on Windows, run `./windows-setup.sh` to fetch needed dependencies
* Run `./resources-build.sh` to build all changed resources
* Run `./linux-build.sh` to build the game for Linux after resource build
* Run `./windows-build.sh` to build the game for Windows after resource build
* Alternatively, create your own `custom-build.sh` based on the Linux or Windows
  build scripts

The following compiler options can be passed in `CFLAGS`:

* `-DDYNAMICLIGHTS` to enable dynamic point lighting
* `-DPROFILE` to enable microsecond profiling

## Usage

After build, run the game in the same way as any other. Either start it from the
command line, or launch it graphically.

## Contributing

Feel free to submit suggestions and bugfixes. Significant user-facing additions
will probably not be accepted if they change the artistic direction of the game.
If you want to make your own game based on Shame on Me, feel free to clone the
repository and do so.
