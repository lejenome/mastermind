INSTALLATION
---

First you need to clone the repo and init its submodules
~~~~~~~~~{.sh}
git clone https://github.com/lejenome/mastermind.git
cd mastermind
git submodule update --init # ext/ submodule is need for android, windows builds
~~~~~~~~~

LINUX
---
Dependencies: `readline, GNU getopt, libncurses5-dev, sdl2, sdl2_ttf, cmake >= 3.0`
To install on Ubuntu, you need ppa that provides a cmake 3.x vez:
~~~~~~~~~{.sh}
apt-add-repository ppa:george-edison55/cmake-3.x
apt-get update
apt-get install cmake libreadline-dev libncurses5 libsdl2-2.0 libsdl2-ttf-dev
~~~~~~~~~
To install on ArchLinux:
~~~~~~~~~{.sh}
pacman -S cmake sdl2 sdl2_ttf ncurses readline
~~~~~~~~~

Next, to build:
~~~~~~~~~{.sh}
mkdir -f build && cd build
cmake ..
make
~~~~~~~~~

To build and run on same dir, you need to set install prefix to build dir until
the build instructions are fixed.
~~~~~~~~~{.sh}
mkdir -f build && cd build
cmake -DCMAKE_INSTALL_PREFIX=. ..
make
./mastermindcli
./mastermindsdl
~~~~~~~~~

OS X
---
Install readline, libncurses, sdl2, sdl2_ttf, cmake from brew

Next, to build:
~~~~~~~~~{.sh}
mkdir -f build && cd build
cmake .. -DUSE_LOCALE=OFF
cmake --build .
~~~~~~~~~

WINDOWS
---
Build is supported using mingw-w64 on all supported hosts (linux, windows,...),
ms visual studio compiler support is still not  usable.
~~~~~~~~~{.sh}
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . -DUSE_LOCALE=OFF
~~~~~~~~~
then you need to copy `../ext/mingw/bin<BUILD_ARCH>/*.dll` libs to build dir.

on same linux platforms, cmake builds with mingw integration are provided. Here
is a demo of a x86_64 mastermind build on ArchLinux using packages provided on
AUR:
~~~~~~~~~{.sh}
yaourt -S mingw-w64-sdl2 mingw-w64-sdl2_ttf mingw-w64-readline mingw-w64-pdcurses mingw-w64-cmake mingw-w64-libiconv
mkdir -f build && cdc build
x86_64-w64-mingw32-cmake ..
make
~~~~~~~~~

ANDROID
---
First install Android NDK and SDK, and install Ant.

Download SDL2, SDL2_ttf and FreeType lastest sources archives and extract them.

then run:
~~~~~~~~~{.sh}
mkdir -p build && cd build
cmake -DUSE_LOCALE=OFF ..
ln -s <PATH_TO_SDL2_SOURCE_DIR> jni/SDL
ln -s <PATH_TO_SDL2_ttf_SOURCE_DIR> jni/SDL_ttf
ln -s <PATH_TO_FreeType_SOURCE_DIR> jni/SDL_ttf/freetype
ndk-build
# you may need to update project target
# e.g: android update project -p . -t android-20
ant release
# gen key and sign apk ile with it, or use your own key
keytool -genkey -v -keystore my-release-key.keystore -alias alias_name -keyalg RSA -keysize 2048 -validity 10000
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore my-release-key.keystore bin/MasterMind-release-unaligned.apk alias_name
# or just use android debug key at $HOME/.android/debug.keystore with alias androiddebugkey and password android
zipalign -v -f 4 bin/MasterMind-release-unaligned.apk bin/MasterMind.apk
~~~~~~~~~
the final apk file is bin/MasterMind.apk

*NOTE*: There is a known issue on SDL2 code caused by the bad GLES 2.0 support.
A temporarily solution until it get fixed on SDL2 code, is to disable GLES 2.0
support, you need to modify `jni/SDL/include/SDL_config_android.h` so `SDL_VIDEO_OPENGL_ES2` is set to 0 :
~~~~~~~~~{.c}
#define SDL_VIDEO_OPENGL_ES2 0
~~~~~~~~~
You may need to set `SDL_VIDEO_RENDER_OGL_ES2` to 0 too :
~~~~~~~~~{.c}
#define SDL_VIDEO_RENDER_OGL_ES2 0
~~~~~~~~~
then rebuild libs and apk starting from ndk-build instruction.
