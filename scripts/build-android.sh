#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Release ..
ln -s ~/SDL2-2.0.3 jni/SDL
ln -s ~/SDL2_ttf-2.0.12 jni/SDL_ttf
ln -s ~/freetype-2.5.5 jni/SDL_ttf/freetype
sed -i 's/#define SDL_VIDEO_OPENGL_ES2 *1/#define SDL_VIDEO_OPENGL_ES2 0/' jni/SDL/include/SDL_config_android.h
android update project -t 1 -p .
ndk-build
ant release > /dev/null
jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 -keystore $HOME/.android/debug.keystore bin/MasterMind-Release-unaligned.apk androiddebugkey < <(echo android)
~/.android/android-sdk.linux/build-tools/21.1.2/zipalign -v -f 4 bin/MasterMind-release-unaligned.apk bin/MasterMind.apk > /dev/null
