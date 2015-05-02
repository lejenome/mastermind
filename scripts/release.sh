#!/bin/bash
# Tested only on ArchLinux with all depencedies intalled and with docker image of
# android toolchain and required android libs added to 
rm -rf release build-linux build-android build-windows build-emscripten

mkdir release build-linux build-android build-windows build-emscripten || exit 1

cd build-linux
cmake -DCMAKE_BUILD_TYPE=Release ..
make all translations doc manual
cpack
cd ..

cd build-windows
CFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	CXXFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	LDFLAGS="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4" \
	x86_64-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
make
cpack
cd ..

docker run -it -v ${PWD}:/mastermind -w /mastermind/build-android \
       	mastermind_android /mastermind/scripts/build-android.sh

cd build-emscripten
emconfigure cmake -DCMAKE_BUILD_TYPE=Release ..
emmake make
cd ..

cp build-linux/MasterMind.{deb,rpm,tar.gz} release/
cp build-android/bin/MasterMind.apk release/
cp build-windows/MasterMind.exe release/
mkdir release/web
cp build-emscripten/mastermindsdl.* release/web
cp release/web/{mastermindsdl,index}.html

cp -r build-linux/manual{,.pdf,.txt} release/
cp -r build-linux/documentation{,.pdf} release/
