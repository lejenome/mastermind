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
First install Android NDK and SDK, and install Ant.

Download SDL2, SDL2_ttf and FreeType lastest sources archives and extract them.

then run:
```shell
mkdir -p build && cd build
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
zipalign -v -f 4 bin/MasterMind-release-unaligned.apk bin/MasterMind.apk
```
the final apk file is bin/MasterMind.apk

*NOTE*: There is a known issue on SDL2 code caused by the bad GLES 2.0 support.
A temporarily solution until it get fixed on SDL2 code, is to disable GLES 2.0
support, you need to modify `jni/SDL/include/SDL_config_android.h` so `SDL_VIDEO_OPENGL_ES2` is set to 0 :
```c
#define SDL_VIDEO_OPENGL_ES2 0
```
You may need to set `SDL_VIDEO_RENDER_OGL_ES2` to 0 too :
```c
#define SDL_VIDEO_RENDER_OGL_ES2 0
```
