outFile "mastermind-installer.exe"

!define APPNAME "MasterMind"
!define DESCRIPTION "Simple and highly customizable MasterMind game implementation"
!define ABOUTURL "https://github.com/lejenome/mastermind"

InstallDir "$PROGRAMFILES\${APPNAME}"
LicenseData "..\LICENSE"

Page directory
Page instfiles

Section "install"
	SetOutPath $INSTDIR
	File "..\README.md"
	File "..\mastermindcli.exe"
	File "..\cygwin1.dll"
	File "..\cygreadline7.dll"
	File "..\cygncursesw-10.dll"
	File "..\cygintl-8.dll"
	File "..\cygiconv-2.dll"

	WriteUninstaller "uninstall.exe"

	# Based on http://nsis.sourceforge.net/A_simple_installer_with_start_menu_shortcut_and_uninstaller
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" "1"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" "1"
SectionEnd

Section "uninstall"
	Delete "$INSTDIR\*"
	rmDir "$INSTDIR"

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
SectionEnd
