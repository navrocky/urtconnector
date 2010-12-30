!define APP_SHORT_NAME "urtconnector"
!define PRODUCT_NAME "UrTConnector"
!define PRODUCT_VERSION "0.6.0"
!define ICON_FILE "urtconnector.ico"
!define USE_COMPRESS

!define PRODUCT_PUBLISHER "URT Community"
!define PRODUCT_WEB_SITE "http://code.google.com/p/urtconnector/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${APP_SHORT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}-${PRODUCT_VERSION}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define START_MENU_ENTRY "${PRODUCT_NAME}"
!define DIR_BIN "..\..\release\src"
!define DIR_TRANS "..\..\release\translations"
!define DIR_IMAGES "..\..\artwork"
!define DIR_SRC "..\..\src"
!define APP_EXE_NAME "${APP_SHORT_NAME}.exe"
!define DESKTOP_LOCAL_LNK "${PRODUCT_NAME}.lnk"

!ifdef USE_COMPRESS
  SetCompressor /SOLID lzma
  SetCompressorDictSize 32
!else
  SetCompress off
!endif

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
;!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP "${IMG_PATH}\vd-install-banner.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${DIR_IMAGES}\install-header.bmp" ; optional
#!define MUI_WELCOMEPAGE_TITLE_3LINES
#!define MUI_FINISHPAGE_TITLE_3LINES

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

; Reserve files
;!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\${APP_SHORT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${APP_SHORT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

InstType "urtconnector"
InstType /NOCUSTOM

Section "UrT Connector"
  SectionIn 1
  SetOutPath "$INSTDIR"
  File "${DIR_BIN}\urtconnector.exe"
  File "${DIR_TRANS}\urtconnector_ru.qm"
  
  File "qstat.exe"
  File "${DIR_SRC}\GeoIP.dat"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtXml4.dll"
  File "QtNetwork4.dll"
  File "mingwm10.dll"
  File "libgcc_s_dw2-1.dll"
  File "libsqlite3.dll"
  !ifdef ICON_FILE
    File "${DIR_IMAGES}\${ICON_FILE}"
  !endif
  CreateDirectory "$SMPROGRAMS\${START_MENU_ENTRY}"
  
  !ifdef ICON_FILE
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${DESKTOP_LOCAL_LNK}" "$INSTDIR\${APP_EXE_NAME}" "" "$INSTDIR\${ICON_FILE}"
    CreateShortCut  "$DESKTOP\${DESKTOP_LOCAL_LNK}" "$INSTDIR\${APP_EXE_NAME}" "" "$INSTDIR\${ICON_FILE}"
  !else
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${DESKTOP_LOCAL_LNK}" "$INSTDIR\${APP_EXE_NAME}"
    CreateShortCut  "$DESKTOP\${DESKTOP_LOCAL_LNK}" "$INSTDIR\${APP_EXE_NAME}"
  !endif

SectionEnd

Section -Additional
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Uninstall.lnk" "$INSTDIR\uninst.exe"

  WriteUninstaller "$INSTDIR\uninst.exe";
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\urtconnector.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  !ifdef ICON_FILE
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${ICON_FILE}"
  !endif
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
SectionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure to uninstall $(^Name)?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$DESKTOP\${DESKTOP_LOCAL_LNK}"
  RMDir /r "$SMPROGRAMS\${START_MENU_ENTRY}"
  RMDir /r /REBOOTOK "$INSTDIR"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
SectionEnd
