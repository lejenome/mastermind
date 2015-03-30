INSTALLATION
---
LINUX
---
Dependencies: `libncurses5-dev, sdl2, cmake >= 3.0`
To install on Ubuntu, you need ppa that provides a cmake 3.x vez:
```shell
apt-add-repository ppa:george-edison55/cmake-3.x
apt-get update
apt-get install cmake libncureses5 libsdl2-2.0
```
To install on ArchLinux:
```shell
pacman -S cmake sdl2 ncurses readline
```

Next, to build:
```shell
mkdir -f build && cd build
cmake ..
make
```
OS X
---
WINDOWS
---
