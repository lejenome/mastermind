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
ANDROID
---
Android build is not supported by CMake yet. You will use SDL2 recommanded way.

First install Android NDK and SDK, and install Ant.

Download SDL2, SDL2_ttf and FreeType lastest sources archives and extract them.

on SDL source dir, enter android-project/ dir. and create symbolic link SDL/ to
SDL2 source dir inside jni/ and an other symbolic link SDL_ttf/ to SDL2_ttf
source dir inside jni/ and a symbolic link freetype to FreeType source dir
inside jni/SDL_ttf and  an other symbolic link mastermind/ to mastermind
source dir inside jni/src/.

Edit jni/src/Android.mk file:
- add `$(LOCAL_PATH)/../SDL_ttf` to LOCAL_C_INCLUDES
- replace YouMainFile.c on LOCAL_SRC_FILES with
`mastermind/src/lib.c mastermind/src/core.c mastermind/src/sdl.c`
- add `SDL2_ttf` to LOCAL_SHARED_LIBRARIES
- add `LOCAL_CFLAGS += -DDISABLE_LOCALE` to the end of file

uncomment `System.loadLibrary("SDL2_ttf");` on
src/org/libsdl/app/SDLActivity.java

then run:
```shell
ndk-build
mkdir -p assert/share/fonts
cp jni/src/mastermind/extra/*.{ttf,pcf} assert/share/fonts
ant release
keytool -genkey -v -keystore my-release-key.keystore -alias alias_name -keyalg RSA -keysize 2048 -validity 10000
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore my-release-key.keystore bin/SDLActivity-release-unsigned.apk alias_name
zipalign -v -f 4 bin/SDLActivity-release-unsigned.apk bin/MasterMind.apk
```
