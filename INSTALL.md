Install
---

First you need to clone the repo and init its submodules
~~~~~~~~~{.sh}
git clone https://github.com/lejenome/mastermind.git
cd mastermind
git submodule update --init # ext/ submodule is need for android, windows builds
~~~~~~~~~
All the following commands assume your current working directory is a child of
the source directory. (eg: `build/` or `build-linux/`,...).
~~~~~~~~~{.sh}
mkdir build
cd build
~~~~~~~~~
And assume that the building direcotry is clean and you didn't run cmake on it
before or you need to clean it.
- [Linux](#linux)
- [OS X](#os-x)
- [Windows](#windows)
- [Android](#android)
- [iOS](#ios)
- [Emscripten](#emscripten)

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
cmake ..
make
~~~~~~~~~

To build and run on same dir, you need to set install prefix to build dir until
the build instructions are fixed.
~~~~~~~~~{.sh}
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
cmake ..
cmake --build .
~~~~~~~~~
For packaging the build on bundle:
~~~~~~~~~{.sh}
cpack
~~~~~~~~~

WINDOWS
---
Build is supported using mingw-w64 on all supported hosts (linux, windows,...)
and using MS Visual Studio on windows.
For build using mingw on windowns:
~~~~~~~~~{.sh}
cmake -G "MinGW Makefiles" ..
cmake --build .
~~~~~~~~~
For build using MS Visual Studio on windows:
~~~~~~~~~{.sh}
cmake ..
cmake --build .
~~~~~~~~~
For packaging the mingw/MS visual studio build on NSIS installer:
~~~~~~~~~{.sh}
cpack
~~~~~~~~~

on same linux platforms, cmake builds with mingw integration are provided. Here
is a demo of a x86_64 mastermind build on ArchLinux using packages provided on
AUR:
~~~~~~~~~{.sh}
yaourt -S mingw-w64-sdl2 mingw-w64-sdl2_ttf mingw-w64-readline mingw-w64-pdcurses mingw-w64-cmake mingw-w64-libiconv
x86_64-w64-mingw32-cmake ..
make
~~~~~~~~~

ANDROID
---
First install Android NDK and SDK, and install Ant.

Download SDL2, SDL2_ttf and FreeType lastest sources archives and extract them.

then run:
~~~~~~~~~{.sh}
cmake ..
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

IOS
---
You need to run cmake to generate config.h
~~~~~~~~~{.sh}
cmake ..
~~~~~~~~~
Building for iOS is currently supported only throw Xcode using the project file
`ide/xcode-ios/MastermindSDL.xcodeproj`.

EMSCRIPTEN
---
You need to have emscripten SDK installed before continuing.
~~~~~~~~~{.sh}
emconfigure cmake ..
emmake make
~~~~~~~~~
Runtime generated files are `mastermindsdl.js`, `mastermindsdl.html`,
`mastermindsdl.data` and `favicon.png`.
