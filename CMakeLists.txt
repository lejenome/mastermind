cmake_minimum_required(VERSION 3.0)
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

Project(MasterMind VERSION 0.1.0 LANGUAGES C)
set(PROJECT_WEBSITE "https://github.com/lejenome/mastermind")

add_executable(mastermindcli src/core.c src/cli-cmd.c src/cli.c)
add_executable(mastermindsdl src/core.c src/sdl.c)

if(MINGW)
	#LIST(APPEND CMAKE_INCLUDE_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/mingw")
	#LIST(APPEND CMAKE_LIBRARY_PATH
	#	"${CMAKE_CURRENT_SOURCE_DIR}/mingw")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		LIST(APPEND CMAKE_INCLUDE_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/mingw/x86_64-w64-mingw32/include")
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/mingw/x86_64-w64-mingw32/lib")
	else(CMAKE_SIZEOF_VOID_P EQUAL 8) # x86
		LIST(APPEND CMAKE_INCLUDE_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/mingw/i686-w64-mingw32/include")
		LIST(APPEND CMAKE_LIBRARY_PATH
			"${CMAKE_CURRENT_SOURCE_DIR}/mingw/i686-w64-mingw32/lib")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
elseif(MSVC)
	LIST(APPEND CMAKE_INCLUDE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/msvc")
	LIST(APPEND CMAKE_LIBRARY_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/msvc")
endif(MINGW)
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

option(USE_LOCALE "Enable internationalization support" ON)
if(USE_LOCALE)
	find_package(Intl)
	find_package(Gettext)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(mastermindcli ${Intl_LIBRARIES})
	target_link_libraries(mastermindsdl ${Intl_LIBRARIES})
	GETTEXT_CREATE_TRANSLATIONS(mastermind.pot po/fr.po)
else(USE_LOCALE)
	message("Building without locale support!")
	add_definitions(-DDISABLE_LOCALE)
endif(USE_LOCALE)

option(DEBUG "Debug Build" OFF)
if(DEBUG)
	add_compile_options(-g)
	add_definitions(-DDEBUG)
endif(DEBUG)
if(CMAKE_C_COMPILER_ID MATCHES "(Clang|GNU|Intel)")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-char-subscripts")
endif(CMAKE_C_COMPILER_ID MATCHES "(Clang|GNU|Intel)")

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_SOURCE_DIR}/config.h"
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
