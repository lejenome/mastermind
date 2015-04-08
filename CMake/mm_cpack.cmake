set(CPACK_PACKAGE_NAME "MasterMind")
set(CPACK_PACKAGE_VENDOR "Dev2Space")
set(CPACK_PACKAGE_VERSION ${MasterMind_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${MasterMind_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MasterMind_VERSION_MINOR})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MasterMind; A simple and highly customizable MasterMind implementation")
set(CPACK_PACKAGE_WEBSITE ${PROJECT_WEBSITE})

set(CPACK_PACKAGE_CONTACT "Moez Bouhlel")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "bmoez.j@gmail.com")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "MasterMind")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CPACK_PACKAGE_DESCRIPTION_FILE}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

#set(CPACK_NSIS_MODIFY_PATH ON)
#set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources\\\\MasterMind_Install.bmp")
#set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources\\\\MasterMind_Icon_96px.ico")
#set(CPACK_NSIS_MUI_UNICON "${CMAKE_SOURCE_DIR}/resource\\\\MasterMind_Icon_96px.ico")

set(CPACK_SOURCE_IGNORE_FILES
	"/\\\\.git/"
	"/intl/"
	"/build/"
	"/CMakeFiles"
	"/_CPack_Packages"
	"/CPackConfig\\\\.cmake$"
	"/CPackSourceConfig\\\\.cmake$"
	"/CMakeCache\\\\.txt$"
	"/cmake_install\\\\.cmake$"
	"/install_manifest\\\\.txt$"
	"/mastermindcli$"
	"/mastermindsdl$"
	"/mastermind\\\\.pot$"
	"/config\\\\.h$"
	"/Makefile$"
	"/.gitignore$"
	"/.clang-format$"
	"\\\\.gmo$"
	"\\\\.o$"
	"\\\\.tar.bz2$"
	"\\\\.tar.gz$"
	"\\\\.tar.Z$"
	"\\\\.tar.xz$"
	"\\\\.deb$"
	"\\\\.rpm$"
	"\\\\.zip$"
	"\\\\.exe$"
	"\\\\.7z$"
) #FIXME ignore .dll on source dir

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-dbg")
endif()

set(CPACK_GENERATOR "DEB;RPM;TGZ;ZIP") # "NSIS;CygwinBinary;PackageMaker;DragNDrop"
set(PACKAGE_SOURCE_GENERATOR "TGZ;ZIP")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libncurses5, libsdl2-2.0") # libsdl2-dev libncurses5-dev # FIXME
set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
set(CPACK_RPM_PACKAGE_REQUIRES "libncurses5, SDL2") # FIXME
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")

set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
configure_file(${CMAKE_ROOT}/Modules/CPack.Info.plist.in Info.plist)
set(CPACK_BUNDLE_PLIST Info.plist)
set(CPACK_BUNDLE_ICON extra/logo.png)

INCLUDE(CPack)
