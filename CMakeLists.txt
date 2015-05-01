cmake_minimum_required(VERSION 3.0)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

Project(MasterMind VERSION 0.1.0 LANGUAGES C)
set(PROJECT_WEBSITE "https://github.com/lejenome/mastermind")

#if(NOT ANDROID AND NOT EMSCRIPTEN AND NOT IPHONEOS)
	add_executable(mastermindcli src/util.c src/core.c src/cli-cmd.c src/cli.c)
#endif(NOT ANDROID AND NOT EMSCRIPTEN AND NOT IPHONEOS)
add_executable(mastermindsdl WIN32 src/util.c src/core.c src/sdl.c)

if(MINGW)
	LIST(APPEND CMAKE_INCLUDE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/include")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/lib64")
		FILE(GLOB win_dll
			${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/bin64/*.dll)
	else(CMAKE_SIZEOF_VOID_P EQUAL 8) # x86
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/lib32")
		FILE(GLOB win_dll
			${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/bin32/*.dll)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
elseif(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
		set_target_properties(mastermindsdl PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
		set_target_properties(mastermindcli PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})
	endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
	# TODO add gettext-runtime{,-dev} from http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/
	LIST(APPEND CMAKE_INCLUDE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/include")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib64")
		FILE(GLOB win_dll
			${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib64/*.dll)
	else(CMAKE_SIZEOF_VOID_P EQUAL 8) # x86
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib32")
		FILE(GLOB win_dll
			${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib32/*.dll)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(MINGW)

if(IOS)
	set(IPHONEOS ON)
endif(IOS)
if(IPHONEOS)
	set(IOS_FRAMEWORKS Foundation AudioToolbox CoreGraphics QuartzCore UIKit OpenGLES)
	set(CMAKE_OSX_SYSROOT iphoneos)
	set(CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos;-iphonesimulator")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD_32_64_BIT)")
	foreach(FW ${IOS_FRAMEWORKS})
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework ${FW}")
	endforeach()
endif(IPHONEOS)

if(NOT EMSCRIPTEN AND NOT IPHONEOS)
	SET(SDL2_SEARCH_TTF ON)
	find_package(SDL2 REQUIRED)
	INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(mastermindsdl ${SDL2_LIBRARY})
elseif(IPHONEOS)
	include_directories(ext/ios/include)
	link_directories(ext/ios/libs/Debug-iphoneos)
	link_directories(ext/ios/libs/Debug-iphonesimulator)
	SET(SDL2_SEARCH_TTF ON)
	file(GLOB SDL2_LIBRARY ext/ios/libs/Debug-iphonesimulator/*)
	TARGET_LINK_LIBRARIES(mastermindsdl ${SDL2_LIBRARY})
elseif(EMSCRIPTEN)
	#SET(CMAKE_C_FLAGS "-s USE_SDL=2 ${CMAKE_C_FLAGS}")
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ext/emscripten/include)
	TARGET_LINK_LIBRARIES(mastermindsdl
		${CMAKE_CURRENT_SOURCE_DIR}/ext/emscripten/libSDL2.a
		${CMAKE_CURRENT_SOURCE_DIR}/ext/emscripten/libSDL2_ttf.a
		${CMAKE_CURRENT_SOURCE_DIR}/ext/emscripten/libfreetype.a
		${CMAKE_CURRENT_SOURCE_DIR}/ext/emscripten/libSDL2main.a)
endif(NOT EMSCRIPTEN AND NOT IPHONEOS)

option(USE_READLINE "Enable readline autocompletetion featues" ON)
option(USE_GETOPT "Enable getopt options parsing support" ON)
option(USE_LOCALE "Enable internationalization support" ON)

if(ANDROID OR EMSCRIPTEN OR IPHONEOS)
	set(USE_READLINE OFF)
	set(USE_GETOPT OFF)
	set(USE_LOCALE OFF)
endif(ANDROID OR EMSCRIPTEN OR IPHONEOS)

find_package(Readline)
if(USE_READLINE AND READLINE_FOUND)
	include_directories(${READLINE_INCLUDE_DIR})
	target_link_libraries(mastermindcli ${READLINE_LIBRARY})
	set(MM_READLINE ON)
else(USE_READLINE AND READLINE_FOUND)
	message("Building without readline support!")
endif(USE_READLINE AND READLINE_FOUND)

INCLUDE (CheckSymbolExists)
CHECK_SYMBOL_EXISTS(getopt_long getopt.h GETOPT_LONG_FOUND)
if(USE_GETOPT AND GETOPT_LONG_FOUND)
	set(MM_GETOPT ON)
endif(USE_GETOPT AND GETOPT_LONG_FOUND)

find_package(Intl)
if(USE_LOCALE AND INTL_FOUND)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(mastermindcli ${Intl_LIBRARIES})
	target_link_libraries(mastermindsdl ${Intl_LIBRARIES})
	set(MM_LOCALE ON)
elseif(USE_LOCALE AND "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	set(MM_LOCALE ON)
else(USE_LOCALE AND INTL_FOUND)
	message("Building without locale support!")
endif(USE_LOCALE AND INTL_FOUND)

include(Gettext)
find_package(Gettext)
if(XGETTEXT_FOUND)
	GETTEXT_CREATE_POT(mastermind.pot SRC src/*.c src/*.h)
	GETTEXT_CREATE_TRANSLATIONS(${CMAKE_BINARY_DIR}/mastermind.pot po/fr.po)
	#GETTEXT_PROCESS_PO_FILES(fr ALL INSTALL_DESTINATION intl PO_FILES po/fr.po)
endif(XGETTEXT_FOUND)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if(CMAKE_C_COMPILER_ID MATCHES "(Clang|GNU|Intel)")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-char-subscripts")
endif(CMAKE_C_COMPILER_ID MATCHES "(Clang|GNU|Intel)")

if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT IPHONEOS AND NOT EMSCRIPTEN)
	INCLUDE(GNUInstallDirs)
endif(UNIX AND NOT ANDROID AND NOT APPLE AND NOT IPHONEOS AND NOT EMSCRIPTEN)
if(NOT CMAKE_INSTALL_BINDIR)
	SET(CMAKE_INSTALL_BINDIR ".")
endif(NOT CMAKE_INSTALL_BINDIR)
if(NOT CMAKE_INSTALL_DATAROOTDIR)
	SET(CMAKE_INSTALL_DATAROOTDIR ".")
endif(NOT CMAKE_INSTALL_DATAROOTDIR)
if(NOT CMAKE_INSTALL_FULL_DATAROOTDIR)
	SET(CMAKE_INSTALL_FULL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
endif(NOT CMAKE_INSTALL_FULL_DATAROOTDIR)
if(NOT CMAKE_INSTALL_LOCALEDIR)
	SET(CMAKE_INSTALL_LOCALEDIR "locale")
	SET(CMAKE_INSTALL_FULL_LOCALEDIR "locale")
endif(NOT CMAKE_INSTALL_LOCALEDIR)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_SOURCE_DIR}/config.h"
)
find_package(Clangformat)
if(CLANG_FORMAT_FOUND)
	message("clang-format found: ${CLANG_FORMAT_EXECUTABLE}")
	add_custom_target(format
		${CLANG_FORMAT_EXECUTABLE} -style=file -i config.h.in src/*.c src/*.h colorscheme/*.h
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

install(DIRECTORY res/fonts DESTINATION ${CMAKE_INSTALL_DATAROOTDIR})
install(FILES res/icons/icon.bmp DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/ RENAME mastermind.bmp)

foreach(dll ${win_dll})
	get_filename_component(dll_name "${dll}" NAME)
	install(FILES "${dll}" DESTINATION .)
	configure_file("${dll}" ${dll_name} COPYONLY)
endforeach(dll)

configure_file(res/fonts/ProFont_r400-29.pcf ${CMAKE_INSTALL_DATAROOTDIR}/fonts/ProFont_r400-29.pcf COPYONLY)
configure_file(res/fonts/fontawesome-webfont.ttf ${CMAKE_INSTALL_DATAROOTDIR}/fonts/fontawesome-webfont.ttf COPYONLY)
configure_file(res/icons/icon.bmp ${CMAKE_INSTALL_DATAROOTDIR}/icons/mastermind.bmp COPYONLY)
if(EMSCRIPTEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --preload-file ${CMAKE_INSTALL_DATAROOTDIR}/fonts/ProFont_r400-29.pcf
	--preload-file ${CMAKE_INSTALL_DATAROOTDIR}/fonts/fontawesome-webfont.ttf --preload-file  ${CMAKE_INSTALL_DATAROOTDIR}/icons/mastermind.bmp")
endif(EMSCRIPTEN)

INCLUDE(sdl2_android)
include(mm_docs)

install(TARGETS mastermindcli DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS mastermindsdl DESTINATION ${CMAKE_INSTALL_BINDIR})
INCLUDE(mm_cpack)
