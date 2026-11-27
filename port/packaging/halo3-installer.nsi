; Halo 3 Native PC Port — Per-User Windows Installer (NSIS 3, Unicode, zero UAC)
;
; Installs to %LOCALAPPDATA%\Programs\Halo 3 without requiring administrator privileges.
; User game data (%LOCALAPPDATA%\halo3) and config (%APPDATA%\Halo3\halo3.ini) are
; preserved across upgrades and uninstalls.
;
; Invoked by port/build/container/build.sh:
;   makensis -DAPP_VERSION=... -DSTAGE_DIR=... -DICON_FILE=... -DOUT_FILE=... halo3-installer.nsi

Unicode True
RequestExecutionLevel user
SetCompressor /SOLID lzma
SetDateSave off

!include "MUI2.nsh"
!include "FileFunc.nsh"

!ifndef APP_VERSION
  !define APP_VERSION "0.0.0"
!endif
!ifndef STAGE_DIR
  !error "STAGE_DIR must be defined"
!endif
!ifndef ICON_FILE
  !error "ICON_FILE must be defined"
!endif
!ifndef OUT_FILE
  !define OUT_FILE "Halo3-Setup.exe"
!endif

!define APP_NAME "Halo 3"
!define APP_FULL_NAME "Halo 3 (Native PC Port)"
!define APP_PUBLISHER "Halo 3 PC Port"
!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Halo3PCPort"

Name "${APP_NAME} ${APP_VERSION}"
Caption "${APP_FULL_NAME} Setup"
OutFile "${OUT_FILE}"
InstallDir "$LOCALAPPDATA\Programs\Halo 3"
InstallDirRegKey HKCU "${UNINST_KEY}" "InstallLocation"
BrandingText "Halo 3 Native PC Port"
ShowInstDetails nevershow
ShowUnInstDetails nevershow

; Modern UI configuration
!define MUI_ICON "${ICON_FILE}"
!define MUI_UNICON "${ICON_FILE}"
!define MUI_ABORTWARNING

!define MUI_WELCOMEPAGE_TITLE "Welcome to Halo 3 Setup"
!define MUI_WELCOMEPAGE_TEXT "This wizard will install ${APP_FULL_NAME} (${APP_VERSION}) for your user account — no administrator permissions required.$\r$\n$\r$\nAfter installation, Halo 3 will guide you through selecting your retail Halo 3 Xbox 360 .iso and preparing your local game data.$\r$\n$\r$\nClick Next to continue."

!define MUI_FINISHPAGE_TITLE "Halo 3 is Installed"
!define MUI_FINISHPAGE_TEXT "${APP_FULL_NAME} has been installed.$\r$\n$\r$\nClick Finish to launch Halo 3 and complete one-time disc setup."
!define MUI_FINISHPAGE_RUN "$INSTDIR\Halo3.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Halo 3"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section "Halo 3 (Required)" SecMain
  SectionIn RO
  SetShellVarContext current
  SetOutPath "$INSTDIR"

  ; Install launcher, metadata, and bin/ tools
  File "${STAGE_DIR}\Halo3.exe"
  File "${STAGE_DIR}\VERSION"
  File "${STAGE_DIR}\SHADER_ABI"
  File "${STAGE_DIR}\BUILD_MANIFEST.txt"

  SetOutPath "$INSTDIR\bin"
  File /r "${STAGE_DIR}\bin\*.*"

  SetOutPath "$INSTDIR"
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\Halo 3"
  CreateShortcut "$SMPROGRAMS\Halo 3\Halo 3.lnk" "$INSTDIR\Halo3.exe" "" "$INSTDIR\Halo3.exe" 0
  CreateShortcut "$SMPROGRAMS\Halo 3\Halo 3 Setup.lnk" "$INSTDIR\Halo3.exe" "--setup" "$INSTDIR\Halo3.exe" 0
  CreateShortcut "$SMPROGRAMS\Halo 3\Uninstall Halo 3.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0

  ; Add / Remove Programs (per-user HKCU)
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegStr   HKCU "${UNINST_KEY}" "DisplayName"     "${APP_FULL_NAME}"
  WriteRegStr   HKCU "${UNINST_KEY}" "DisplayVersion"  "${APP_VERSION}"
  WriteRegStr   HKCU "${UNINST_KEY}" "Publisher"       "${APP_PUBLISHER}"
  WriteRegStr   HKCU "${UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr   HKCU "${UNINST_KEY}" "DisplayIcon"     "$INSTDIR\Halo3.exe,0"
  WriteRegStr   HKCU "${UNINST_KEY}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr   HKCU "${UNINST_KEY}" "QuietUninstallString" '"$INSTDIR\Uninstall.exe" /S'
  WriteRegDWORD HKCU "${UNINST_KEY}" "NoModify"        1
  WriteRegDWORD HKCU "${UNINST_KEY}" "NoRepair"        1
  WriteRegDWORD HKCU "${UNINST_KEY}" "EstimatedSize"   $0
SectionEnd

Section "Desktop Shortcut" SecDesktop
  SetShellVarContext current
  CreateShortcut "$DESKTOP\Halo 3.lnk" "$INSTDIR\Halo3.exe" "" "$INSTDIR\Halo3.exe" 0
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "Installs Halo 3, the native setup launcher, ISO extractor, and Vulkan shader translator."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Places a Halo 3 shortcut on your Desktop."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"
  SetShellVarContext current

  ; Remove shortcuts
  Delete "$DESKTOP\Halo 3.lnk"
  Delete "$SMPROGRAMS\Halo 3\Halo 3.lnk"
  Delete "$SMPROGRAMS\Halo 3\Halo 3 Setup.lnk"
  Delete "$SMPROGRAMS\Halo 3\Uninstall Halo 3.lnk"
  RMDir  "$SMPROGRAMS\Halo 3"

  ; Remove installed program binaries (user saves/data in %LOCALAPPDATA%\halo3 are preserved)
  Delete "$INSTDIR\Halo3.exe"
  Delete "$INSTDIR\Halo3.exe.old"
  Delete "$INSTDIR\VERSION"
  Delete "$INSTDIR\SHADER_ABI"
  Delete "$INSTDIR\BUILD_MANIFEST.txt"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir /r "$INSTDIR\bin"
  RMDir "$INSTDIR"

  DeleteRegKey HKCU "${UNINST_KEY}"
SectionEnd
