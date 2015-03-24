# based on Archlinux recommondations: https://wiki.archlinux.org/index.php/MinGW_package_guidelines
# TODO: AUR mingw-w64-gettext, mingw-w64-sdl2, mingw-w64-readline
SET(CMAKE_SYSTEM_NAME Windows)
SET(_mingw_w64 "x86_64-w64-mingw32")
#SET(_mingw_w64 "i686-w64-mingw32")
SET(CMAKE_C_FLAGS "-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4")

SET(CMAKE_C_COMPILER ${_mingw_w64}-gcc)
SET(CMAKE_CXX_COMPILER ${_mingw_w64}-g++)
SET(CMAKE_RC_COMPILER ${_mingw_w64}-windres)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH "/usr/${_mingw_w64}")

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
