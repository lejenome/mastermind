set(CPACK_PACKAGE_NAME "MasterMind")
set(CPACK_PACKAGE_VENDOR "Dev2Space")
set(CPACK_PACKAGE_VERSION "${MasterMind_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${MasterMind_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MasterMind_VERSION_MINOR})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MasterMind; A simple and highly customizable MasterMind implementation")

set(CPACK_PACKAGE_CONTACT "Moez Bouhlel")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "bmoez.j@gmail.com")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "MasterMind")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_BINARY_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CPACK_PACKAGE_DESCRIPTION_FILE}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE")

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
)

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}")
SET(CPACK_GENERATOR "DEB;RPM;TGZ;ZIP") # "NSIS"
set(PACKAGE_SOURCE_GENERATOR "TGZ;ZIP")
INCLUDE(CPack)
