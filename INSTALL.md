INSTALLATION
---
LINUX
---
Dependencies: `readline, GNU getopt, libncurses5-dev, sdl2, sdl2_ttf, cmake >= 3.0`
To install on Ubuntu, you need ppa that provides a cmake 3.x vez:
```shell
apt-add-repository ppa:george-edison55/cmake-3.x
apt-get update
apt-get install cmake libreadline-dev libncurses5 libsdl2-2.0 libsdl2-ttf-dev
```
To install on ArchLinux:
```shell
pacman -S cmake sdl2 sdl2_ttf ncurses readline
```

Next, to build:
```shell
mkdir -f build && cd build
cmake ..
make
```

To run the build, (simple fix until I fix the code)
```shell
mkdir -p share/fonts && cp ../extra/* share/fonts
./mastermindcli
./mastermindsdl
```

OS X
---
WINDOWS
---
