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
set(PACKAGE_SOURCE_GENERATOR "TGZ;ZIP")
if(WIN32)
	list(APPEND CPACK_GENERATOR NSIS NSIS64 ZIP) # CygwinBinairy
elseif(APPLE)
	list(APPEND CPACK_GENERATOR Bundle) # OSXX11 DragNDrop PackageMaker
else()
	list(APPEND CPACK_GENERATOR DEB RPM TGZ)
endif()

# DEB & RPM
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libncurses5, libsdl2-2.0-0") # libsdl2-dev libncurses5-dev # FIXME
set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
set(CPACK_RPM_PACKAGE_REQUIRES "libncurses5, SDL2") # FIXME
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")

if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	configure_file(res/mastermindsdl.desktop.in mastermindsdl.desktop)
	install(FILES ${CMAKE_BINARY_DIR}/mastermindsdl.desktop DESTINATION share/applications/)
	install(FILES res/icons/logo-16x16.png DESTINATION share/icons/hicolor/16x16/apps/ RENAME mastermindsdl.png)
	install(FILES res/icons/logo-32x32.png DESTINATION share/icons/hicolor/32x32/apps/ RENAME mastermindsdl.png)
	install(FILES res/icons/logo-48x48.png DESTINATION share/icons/hicolor/48x48/apps/ RENAME mastermindsdl.png)
	install(FILES res/icons/logo-256x256.png DESTINATION share/icons/hicolor/256x256/apps/ RENAME mastermindsdl.png)
	install(FILES doc/mastermindcli.1 DESTINATION share/man/man1/)
	install(FILES doc/config.md DESTINATION share/doc/mastermind/)
	install(FILES LICENSE DESTINATION share/licenses)
	install(FILES res/completion/mastermindcli.zsh DESTINATION share/zsh/functions/Completion/Unix/ RENAME _mastermindcli) #PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
	install(FILES res/completion/mastermindcli.bash DESTINATION share/bash-completion/completions/ RENAME mastermindcli)
else("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	install(FILES res/icons/logo.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/ RENAME mastermind.png)
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")

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

# EMSCRIPTEN
if(EMSCRIPTEN)
	set(EMSCRIPTEN_JS_FILE mastermindsdl.js)
	configure_file(ext/emscripten/template.html.in mastermindsdl.html @ONLY)
	#set(EMSCRIPTEN_JS_FILE mastermindcli.js)
	#configure_file(ext/emscripten/template.html.in mastermindcli.html @ONLY)
	configure_file(res/icons/logo-16x16.png favicon.png COPYONLY)
	#install(COPY ext/emscripten/loading.png .)
endif(EMSCRIPTEN)

# IOS BUNDLE
if(IPHONEOS)
	set_target_properties(
	    mastermindsdl
	    PROPERTIES
	    MACOSX_BUNDLE YES
	    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_BINARY_DIR}/Info.plist"
	    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
	    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
	    XCODE_ATTRIBUTE_INSTALL_PATH "${CMAKE_BINARY_DIR}/ProductRelease"
	    XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
	    #RESOURCE "${IMAGES};${STORYBOARDS};${ICONS}"
	)
	set_target_properties(
	    mastermindsdl
	    PROPERTIES
	    XCODE_ATTRIBUTE_PRODUCT_NAME
	    "MasterMind"
	    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER
	    "com.dev2space.mastermind"
	)
	set_target_properties(
	    mastermindsdl
	    PROPERTIES
	    XCODE_ATTRIBUTE_PRODUCT_NAME[variant=Debug]
	    "MasterMind-dbg"
	    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER[variant=Debug]
	    "com.dev2space.mastermind.debug"
	)
endif(IPHONEOS)
INCLUDE(CPack)
