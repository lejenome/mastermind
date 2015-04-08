cmake_minimum_required(VERSION 3.0)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

Project(MasterMind VERSION 0.1.0 LANGUAGES C)
set(PROJECT_WEBSITE "https://github.com/lejenome/mastermind")

add_executable(mastermindcli src/lib.c src/core.c src/cli-cmd.c src/cli.c)
add_executable(mastermindsdl src/lib.c src/core.c src/sdl.c)

if(MINGW)
	#LIST(APPEND CMAKE_INCLUDE_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/mingw")
	#LIST(APPEND CMAKE_LIBRARY_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/mingw")
	LIST(APPEND CMAKE_INCLUDE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/include")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/lib64")
	else(CMAKE_SIZEOF_VOID_P EQUAL 8) # x86
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/mingw/lib32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
elseif(MSVC)
	# TODO add gettext-runtime{,-dev} from http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/
	#LIST(APPEND CMAKE_INCLUDE_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/msvc")
	#LIST(APPEND CMAKE_LIBRARY_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/msvc")
	LIST(APPEND CMAKE_INCLUDE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/include")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib64")
	else(CMAKE_SIZEOF_VOID_P EQUAL 8) # x86
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/ext/msvc/lib32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif(MINGW)
SET(SDL2_SEARCH_TTF ON)
find_package(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(mastermindsdl ${SDL2_LIBRARY})

option(USE_READLINE "Enable readline autocompletetion featues" ON)
if(USE_READLINE)
	find_package(Readline REQUIRED)
	include_directories(${READLINE_INCLUDE_DIR})
	target_link_libraries(mastermindcli ${READLINE_LIBRARY})
else(USE_READLINE)
	message("Building without readline support!")
	add_definitions(-DDISABLE_READLINE)
endif(USE_READLINE)

option(USE_GETOPT "Enable getopt options parsing support" ON)
if(NOT USE_GETOPT)
	add_definitions(-DDISABLE_GETOPT)
endif(NOT USE_GETOPT)

option(USE_LOCALE "Enable internationalization support" ON)
if(USE_LOCALE)
	find_package(Intl)
	include(Gettext)
	find_package(Gettext)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(mastermindcli ${Intl_LIBRARIES})
	target_link_libraries(mastermindsdl ${Intl_LIBRARIES})
else(USE_LOCALE)
	message("Building without locale support!")
	add_definitions(-DDISABLE_LOCALE)
endif(USE_LOCALE)

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

if(WIN32)
elseif(APPLE)
else()
	INCLUDE(GNUInstallDirs)
endif(WIN32)
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
	add_custom_target(format ALL
		${CLANG_FORMAT_EXECUTABLE} -style=file -i config.h.in src/*.c src/*.h colorscheme/*.h
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

install(FILES extra/fontawesome-webfont.ttf DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/fonts/)
install(FILES extra/ProFont_r400-29.pcf DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/fonts/)

configure_file(extra/ProFont_r400-29.pcf ${CMAKE_INSTALL_DATAROOTDIR}/fonts/ProFont_r400-29.pcf COPYONLY)
configure_file(extra/fontawesome-webfont.ttf ${CMAKE_INSTALL_DATAROOTDIR}/fonts/fontawesome-webfont.ttf COPYONLY)

INCLUDE(sdl2_android)

install(TARGETS mastermindcli DESTINATION bin)
install(TARGETS mastermindsdl DESTINATION bin)
INCLUDE(mm_cpack)
