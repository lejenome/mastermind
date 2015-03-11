outFile "mastermind-installer.exe"

!define APPNAME "MasterMind"
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
SectionEnd

Section "uninstall"
	Delete "$INSTDIR\*"
	rmDir "$INSTDIR"
SectionEnd
