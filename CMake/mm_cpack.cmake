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

set(CPACK_PACKAGE_RELOCATABLE false) # FIXME
set(CPACK_GENERATOR "TGZ;ZIP")
set(PACKAGE_SOURCE_GENERATOR "TGZ;ZIP")
if(WIN32)
	list(APPEND CPACK_GENERATOR NSIS NSIS64) # CygwinBinairy
elseif(APPLE)
	list(APPEND CPACK_GENERATOR Bundle) # OSXX11 DragNDrop PackageMaker
else()
	list(APPEND CPACK_GENERATOR DEB RPM)
endif()

# DEB & RPM
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libncurses5, libsdl2-2.0") # libsdl2-dev libncurses5-dev # FIXME
set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
set(CPACK_RPM_PACKAGE_REQUIRES "libncurses5, SDL2") # FIXME
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")

# BUNDLE
set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
set(CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/logo.icns)
configure_file(CMake/CPack.Info.plist.in Info.plist)
set(CPACK_BUNDLE_PLIST Info.plist)
set(CPACK_BUNDLE_STARTUP_COMMAND mastermindsdl)

# OSXX11
if(APPLE)
	install(FILES res/icons/logo.icns DESTINATION icons)
endif(APPLE)
set(CPACK_APPLE_GUI_INFO_STRING ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
set(CPACK_APPLE_GUI_ICON icons/logo.icns)
set(CPACK_APPLE_GUI_IDENTIFIER com.${CPACK_PACKAGE_VENDOR}.${CPACK_PACKAGE_NAME})
set(CPACK_APPLE_GUI_LONG_VERSION_STRING ${CPACK_PACKAGE_VERSION})
set(CPACK_APPLE_GUI_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_APPLE_GUI_SHORT_VERSION_STRING ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
set(CPACK_APPLE_GUI_BUNDLE_VERSION ${CPACK_APPLE_GUI_SHORT_VERSION_STRING})
set(CPACK_APPLE_GUI_COPYRIGHT "Copyright 2015 ${CPACK_PACKAGE_CONTACT}.")

# NSIS
if(WIN32)
	install(FILES res/icons/icon.ico DESTINATION icons)
endif(WIN32)
set(CPACK_NSIS_MUI_ICON ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/icon.ico)
set(CPACK_NSIS_CREATE_ICONS_EXTRA "
	CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Mastermind SDL.lnk\\\" \\\"$INSTDIR\\\\mastermindsdl.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\icons\\\\icon.ico\\\" \n
	CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Mastermind cli.lnk\\\" \\\"$INSTDIR\\\\mastermindcli.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\icons\\\\icon.ico\\\" \n
	CreateShortCut \\\"$DESKTOP\\\\Mastermind SDL.lnk\\\" \\\"$INSTDIR\\\\mastermindsdl.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\icons\\\\icon.ico\\\" \n
	"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA "
	Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Mastermind SDL.lnk\\\" \n
	Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Mastermind cli.lnk\\\" \n
	Delete \\\"$DESKTOP\\\\Mastermind SDL.lnk\\\" \n
	"
)
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN mastermindsdl)

INCLUDE(CPack)
