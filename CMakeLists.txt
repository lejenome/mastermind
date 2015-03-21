cmake_minimum_required(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

Project(MasterMind)
set(MasterMind_VERSION_MAJOR 0)
set(MasterMind_VERSION_MINOR 1)

add_executable(mastermindcli src/core.c src/cli-cmd.c src/cli.c)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(mastermindcli ${SDL2_LIBRARIES})

option(USE_READLINE "Enable readline autocompletetion featues" ON)
if(USE_READLINE)
	find_package(Readline REQUIRED)
	include_directories(${READLINE_INCLUDE_DIR})
	target_link_libraries(mastermindcli ${READLINE_LIBRARY})
else(USE_READLINE)
	message("Building without readline support!")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_READLINE")
endif(USE_READLINE)

option(USE_LOCALE "Enable internationalization support" ON)
if(USE_LOCALE)
	find_package(Gettext)
else(USE_LOCALE)
	message("Building without locale support!")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDISABLE_LOCALE")
endif(USE_LOCALE)

option(DEBUG "Debug Build" OFF)
if(DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEBUG -g")
endif(DEBUG)

GETTEXT_CREATE_TRANSLATIONS(mastermind.pot ALL po/fr.po)

install(TARGETS mastermindcli DESTINATION bin)
INCLUDE(mm_cpack)
