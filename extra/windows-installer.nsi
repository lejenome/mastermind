!include "MUI2.nsh"

outFile "mastermind-installer.exe"

!define MUI_PRODUCT "MasterMind"
!define DESCRIPTION "Simple and highly customizable MasterMind game implementation"
!define ABOUTURL "https://github.com/lejenome/mastermind"
#!define MUI_ICON "mastermind.ico"
#!define MUI_UNICON "mastermind.ico"
Name "${MUI_PRODUCT}"

InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"

RequestExecutionLevel user

!define MUI_PAGE_HEADER_TEXT "MasterMind game"
!define MUI_PAGE_HEADER_SUBTEXT "Simple and highly customizable Game"
!define MUI_WELCOMEPAGE_TITLE "MasterMind Game"
!define MUI_WELCOMEPAGE_TEXT "Welcome to the MasterMind Installer!"

#Page directory
#Page instfiles
# http://nsis.sourceforge.net/VLC_media_player
# Install config
!define MUI_FINISHPAGE_RUN "$INSTDIR\mastermindcli.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_LINK_LOCATION "${ABOUTURL}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
# Uninstall config
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "install"
	SetOutPath $INSTDIR
	File /oname=README.txt "..\README.md"
	File /oname=LICENSE.txt "..\LICENSE"
	File "..\mastermindcli.exe"
	File "..\cygwin1.dll"
	File "..\cygreadline7.dll"
	File "..\cygncursesw-10.dll"
	File "..\cygintl-8.dll"
	File "..\cygiconv-2.dll"

	WriteUninstaller "uninstall.exe"

	# http://nsis.sourceforge.net/Sample_installation_script_for_an_application
	CreateShortCut "$DESKTOP\mastermindcli.lnk" "$INSTDIR\mastermindcli.exe" ""
	CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
	CreateShortCut "$SMPROGRAMS\${MUI_PRODUCT}\mastermindcli.lnk" "$INSTDIR\mastermindcli.exe" "" "$INSTDIR\mastermindcli.exe" 0
	# Based on http://nsis.sourceforge.net/A_simple_installer_with_start_menu_shortcut_and_uninstaller
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "DisplayName" "${MUI_PRODUCT} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "NoModify" "1"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}" "NoRepair" "1"
SectionEnd

Section "uninstall"
	Delete "$INSTDIR\*"
	rmDir "$INSTDIR"

	Delete "$DESKTOP\mastermindcli.lnk"
	Delete "$SMPROGRAMS\${MUI_PRODUCT}\*.*"
	RmDir  "$SMPROGRAMS\${MUI_PRODUCT}"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"

	ReadRegStr $0 HKU "$1\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" AppData
	RmDir /r "$0\mastermind"
SectionEnd
