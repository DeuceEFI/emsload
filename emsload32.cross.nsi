; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "EMSLoad"

; The file to write
OutFile "EMSLoadInstaller.exe"

; The default installation directory
InstallDir $PROGRAMFILES\EMSLoad

InstallDirRegKey HKLM "Software\EMSLoad" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "EMSLoad (Required)" ;No components page, name is not important

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  ; These files should be local to this script
  ;File "/home/michael/code/emstudio/release/emstudio.exe"
  ;File "/home/michael/code/emstudio/src/gauges.qml"
  ;File "/home/michael/code/emstudio/freeems.config.json"
  File "release/emsload.exe"
  
  

; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\EMSLoad "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EMSLoad" "DisplayName" "EMSLoad"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EMSLoad" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EMSLoad" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EMSLoad" "NoRepair" 1
  WriteUninstaller "uninstall.exe"  
SectionEnd ; end the section

Section "Qt Components"

  SetOutPath $INSTDIR

  File /home/michael/QtWin32/lib/QtCore4.dll
  File /home/michael/QtWin32/lib/QtGui4.dll
;  File /home/michael/QtWin32/lib/QtOpenGL4.dll
;  File /home/michael/QtWin32/lib/QtSvg4.dll
;  File /home/michael/QtWin32/lib/QtDeclarative4.dll
;  File /home/michael/QtWin32/lib/QtGui4.dll
;  File /home/michael/QtWin32/lib/QtNetwork4.dll
;  File /home/michael/QtWin32/lib/QtScript4.dll
;  File /home/michael/QtWin32/lib/QtSql4.dll
;  File /home/michael/QtWin32/lib/QtXml4.dll
;  File /home/michael/QtWin32/lib/QtXmlPatterns4.dll
;  File /home/michael/QtWin32/libs/qwt/bin/qwt.dll
;  File /home/michael/QtWin32/libs/qjson/bin/qjson.dll
  File /home/michael/QtWin32/lib/libgcc_s_sjlj-1.dll
  File /home/michael/QtWin32/lib/libstdc++-6.dll
  File /home/michael/QtWin32/lib/libwinpthread-1.dll

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\EMSLoad"
  CreateShortCut "$SMPROGRAMS\EMSLoad\Uninstall.lnk" "$INSTDIR\emsloaduninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\EMSLoad\EMSLoad.lnk" "$INSTDIR\EMSLoad.exe" "" "$INSTDIR\EMSLoad.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EMSLoad"
  DeleteRegKey HKLM SOFTWARE\EMSLoad

  ; Remove files and uninstaller
  Delete $INSTDIR\EMSLoad.exe
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\EMSLoad\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\EMSLoad"
  RMDir "$INSTDIR"

SectionEnd
