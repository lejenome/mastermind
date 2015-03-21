cmake_minimum_required(VERSION 3.0)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

Project(MasterMind VERSION 0.1.0 LANGUAGES C)

add_executable(mastermindcli src/core.c src/cli-cmd.c src/cli.c)
add_executable(mastermindsdl src/core.c src/sdl.c)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(mastermindsdl ${SDL2_LIBRARIES})

option(USE_READLINE "Enable readline autocompletetion featues" ON)
if(USE_READLINE)
	find_package(Readline REQUIRED)
	include_directories(${READLINE_INCLUDE_DIR})
	target_link_libraries(mastermindcli ${READLINE_LIBRARY})
else(USE_READLINE)
	message("Building without readline support!")
	add_definitions(-DDISABLE_READLINE)
endif(USE_READLINE)

option(USE_LOCALE "Enable internationalization support" ON)
if(USE_LOCALE)
	find_package(Gettext)
else(USE_LOCALE)
	message("Building without locale support!")
	add_definitions(-DDISABLE_LOCALE)
endif(USE_LOCALE)

option(DEBUG "Debug Build" OFF)
if(DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG -g")
endif(DEBUG)

GETTEXT_CREATE_TRANSLATIONS(mastermind.pot po/fr.po)

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)
find_package(Clangformat)
if(CLANG_FORMAT_FOUND)
	message("clang-format found: ${CLANG_FORMAT_EXECUTABLE}")
	add_custom_target(format ALL
		${CLANG_FORMAT_EXECUTABLE} -style=file -i config.h.in src/*.c src/*.h
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif(CLANG_FORMAT_FOUND)

install(TARGETS mastermindcli DESTINATION bin)
install(TARGETS mastermindsdl DESTINATION bin)
INCLUDE(mm_cpack)
