
!define APP_SHORT_NAME "urtconnector"
!define PRODUCT_NAME "Urban Terror Connector"
!define PRODUCT_VERSION "0.2.0"
!define ICON_FILE "app-icon.ico"
!define USE_COMPRESS


!define PRODUCT_PUBLISHER "Vladislav Navrocky"
!define PRODUCT_WEB_SITE "http://code.google.com/p/urtconnector/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${APP_SHORT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}-${PRODUCT_VERSION}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define START_MENU_ENTRY "${PRODUCT_NAME}"
!define DIR_BIN "..\..\release\src"
!define DIR_TRANS "..\..\release\translations"

;!define DIR_APP "common\app"
;!define DIR_DATA "common\data"
;!define DIR_DEMO_DATA "demo\data"
;!define DIR_IMAGES "images"

;!define APP_EXE_NAME "${APP_SHORT_NAME}.exe"

!define DESKTOP_LOCAL_LNK "${PRODUCT_NAME}.lnk"
;!define DESKTOP_CLIENT_LNK "${PRODUCT_NAME} Клиент.lnk"

Var APP_PATH

!ifdef USE_COMPRESS
  SetCompressor /SOLID lzma
  SetCompressorDictSize 32
!else
  SetCompress off
!endif

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define IMG_PATH "..\..\vd_install_common\images"
!define MUI_ABORTWARNING
;!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_ICON "${IMG_PATH}\vd-icon.ico"
;!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_UNICON "${IMG_PATH}\vd-uninstall.ico"
;!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP "${IMG_PATH}\vd-install-banner.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
;!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp" ; optional
#!define MUI_HEADERIMAGE_BITMAP "${IMG_PATH}\vd-install-header.bmp" ; optional
#!define MUI_WELCOMEPAGE_TITLE_3LINES
#!define MUI_FINISHPAGE_TITLE_3LINES

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "${DIR_BIN}\license.rtf"

!ifndef USE_DEMO
!ifndef DONT_USE_CLIENT_SERVER
  !insertmacro MUI_PAGE_COMPONENTS
!endif
!endif

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
;!insertmacro XPUI_LANGUAGE "English"
;!insertmacro MUI_LANGUAGE "Russian"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\${APP_SHORT_NAME}-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\VisualData\${APP_SHORT_NAME}"
;InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show


  InstType "urtconnector"
  InstType /NOCUSTOM

;!ifdef USE_DEMO
;  InstType "Демонстрация"
;  InstType /NOCUSTOM
;!else
;  !ifndef DONT_USE_LOCAL
;    InstType "Локальная работа"
;    !ifdef DONT_USE_CLIENT_SERVER
;      InstType /NOCUSTOM
;    !endif
;  !endif
;  !ifndef DONT_USE_CLIENT_SERVER
;    InstType "Клиент"
;    InstType "Сервер (системная служба)"
;    InstType "Сервер (приложение)"
;    InstType "Сервер (консольный вариант)"
;  !endif
;!endif
;InstType /COMPONENTSONLYONCUSTOM

; Detection of Qt libs placement
Function DetectQt

FunctionEnd



Section "UrT Connector"
  SectionIn 1
  SetOutPath "$INSTDIR"
  File "${DIR_BIN}\urtconnector.exe"
  File "${DIR_BIN}\urtconnector_ru.qm"
  
  File "${DIR_BIN}\vdjobman.dll"
  File "${DIR_BIN}\reportview.exe"
  File "${DIR_BIN}\vd.vdr"
  File "${DIR_BIN}\extimg.vdr"
  File "${DIR_BIN}\log.dll"
  File "${DIR_BIN}\mm.dll"
  File "${DIR_BIN}\padeg.dll"
  File "${DIR_BIN}\config.vdr"
  !ifdef ICON_FILE
    File "${DIR_IMAGES}\${ICON_FILE}"
  !endif
  !ifdef USE_DEMO
    File /oname=config.ini "${DIR_BIN}\config_demo.ini"
  !else
    File /oname=config.ini "${DIR_BIN}\config_local.ini"
  !endif
  CreateDirectory "$SMPROGRAMS\${START_MENU_ENTRY}"
  
  !ifdef ICON_FILE
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${DESKTOP_LOCAL_LNK}" "$APP_PATH\${APP_EXE_NAME}" "" "$APP_PATH\${ICON_FILE}"
    CreateShortCut  "$DESKTOP\${DESKTOP_LOCAL_LNK}" "$APP_PATH\${APP_EXE_NAME}" "" "$APP_PATH\${ICON_FILE}"
  !else
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${DESKTOP_LOCAL_LNK}" "$APP_PATH\${APP_EXE_NAME}"
    CreateShortCut  "$DESKTOP\${DESKTOP_LOCAL_LNK}" "$APP_PATH\${APP_EXE_NAME}"
  !endif

  !ifdef APP_FORMS_NAME
    SetOutPath "$APP_PATH\Forms"
    File "${DIR_APP}\${APP_FORMS_NAME}"
  !else
    File /oname=scenario.stg "${DIR_APP}\ScenarioDesigner.stg"
  !endif

  SetOutPath "$APP_PATH\Meta"
  File /oname=MetaAuto.xml "${DIR_APP}\MetaLocal.xml"

  call InstallBase
SectionEnd

!ifndef USE_DEMO & DONT_USE_CLIENT_SERVER
Section "Клиент"
  SectionIn 2
  StrCpy $APP_PATH "$INSTDIR\Client"
  call TryUpdate
  !ifdef APP_FORMS_NAME
    SetOutPath "$APP_PATH\Forms"
    File "${DIR_APP}\${APP_FORMS_NAME}"
  !else
    SetOutPath "$APP_PATH"
    File /oname=scenario.stg "${DIR_APP}\ScenarioDesigner.stg"
  !endif
  SetOutPath "$APP_PATH\Meta"
    File /oname=MetaClient.xml "${DIR_APP}\MetaLocal.xml"
  SetOutPath "$APP_PATH"
  File /oname=${APP_EXE_NAME} "${DIR_BIN}\visualdata.exe"
  File "${DIR_BIN}\serverconsole.exe"
  File "${DIR_BIN}\reportview.exe"
  File "${DIR_BIN}\vdjobman.dll"
  File "${DIR_BIN}\mm.dll"
  File "${DIR_BIN}\padeg.dll"
  File "${DIR_BIN}\vd.vdr"
  File "${DIR_BIN}\extimg.vdr"
  File "${DIR_BIN}\config.vdr"
  File "${DIR_BIN}\xdelta.exe"
  !ifdef ICON_FILE
    File "${DIR_IMAGES}\${ICON_FILE}"
  !endif
  File /oname=config.ini "${DIR_BIN}\config_client.ini"
  CreateDirectory "$SMPROGRAMS\${START_MENU_ENTRY}"
  !ifdef ICON_FILE
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${PRODUCT_NAME} Клиент.lnk" "$APP_PATH\${APP_EXE_NAME}" "" "$APP_PATH\${ICON_FILE}"
    CreateShortCut "$DESKTOP\${DESKTOP_CLIENT_LNK}" "$APP_PATH\${APP_EXE_NAME}" "" "$APP_PATH\${ICON_FILE}"
  !else
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\${PRODUCT_NAME} Клиент.lnk" "$APP_PATH\${APP_EXE_NAME}"
    CreateShortCut "$DESKTOP\${DESKTOP_CLIENT_LNK}" "$APP_PATH\${APP_EXE_NAME}"
  !endif
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Консоль управления сервером.lnk" "$APP_PATH\serverconsole.exe"
SectionEnd

Function ServerCommon
  StrCpy $APP_PATH "$INSTDIR\Server"
  call TryUpdate
  SetOutPath "$APP_PATH"
    File "${DIR_BIN}\log.dll"
    File "${DIR_BIN}\mm.dll"
    File "${DIR_BIN}\vd.vdr"
    File "${DIR_BIN}\padeg.dll"
    File "${DIR_BIN}\config.vdr"
    File "${DIR_BIN}\xdelta.exe"
    File /oname=config.ini "${DIR_BIN}\config_server.ini"
  CreateDirectory "$SMPROGRAMS\${START_MENU_ENTRY}"

  call InstallBase

  SetOutPath "$APP_PATH\Meta"
    File /oname=MetaAuto.xml "${DIR_APP}\MetaLocal.xml"

  ;Файлы для клиента
  SetOutPath "$APP_PATH\Client\Update"
    File /oname=${APP_EXE_NAME} "${DIR_BIN}\visualdata.exe"
    File "${DIR_BIN}\reportview.exe"
    File "${DIR_BIN}\vdjobman.dll"
    File "${DIR_BIN}\mm.dll"
    File "${DIR_BIN}\padeg.dll"
    File "${DIR_BIN}\vd.vdr"
    File "${DIR_BIN}\extimg.vdr"
    File "${DIR_BIN}\xdelta.exe"
  !ifdef APP_FORMS_NAME
    SetOutPath "$APP_PATH\Client\Update\Forms"
    File "${DIR_APP}\${APP_FORMS_NAME}"
  !else
    File /oname=scenario.stg "${DIR_APP}\ScenarioDesigner.stg"
  !endif

  SetOutPath "$APP_PATH\Client\Init"
    File "${DIR_BIN}\config.vdr"
    File /oname=config.ini "${DIR_BIN}\config_client.ini"
FunctionEnd

Section "Сервер (системная служба)"
  SectionIn 3
  call ServerCommon
  ;AddSize 14500
  SetOutPath "$APP_PATH"
    File /oname=serversvc.exe "${DIR_BIN}\vdservice.exe"
    File "${DIR_BIN}\serverconsole.exe"
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Консоль управления сервером.lnk" "$APP_PATH\serverconsole.exe"

  ; устанавливаем сервис как службу
  ExecWait '"$APP_PATH\serversvc.exe" /name "vd_${APP_SHORT_NAME}" "${PRODUCT_NAME}" "Сервер задачи ${PRODUCT_NAME}. Используйте serverconsole.exe для управления." /install /silent'

  ; запускаем сервис
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "Сервер установлен как системная служба. Запустить сервер сейчас?" IDNO no
    ExecWait '"$APP_PATH\serversvc.exe" /name "vd_${APP_SHORT_NAME}" /start /silent'
  no:
SectionEnd

Section "Сервер (приложение)"
  SectionIn 4
  call ServerCommon
  ;AddSize 14500
  SetOutPath "$APP_PATH"
    File /oname=server.exe "${DIR_BIN}\vdserver.exe"
    File "${DIR_BIN}\vdjobman.dll"
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Сервер.lnk" "$APP_PATH\server.exe"
SectionEnd

Section "Сервер (консольный вариант)"
  SectionIn 5
  call ServerCommon
  ;AddSize 14500
  SetOutPath "$APP_PATH"
    File /oname=consserver.exe "${DIR_BIN}\vdconsserv.exe"
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Сервер.lnk" "$APP_PATH\consserver.exe"
SectionEnd

!endif

Section -Additional
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Удаление программы.lnk" "$INSTDIR\uninst.exe"
  SetOutPath "$INSTDIR"
  File "${DIR_BIN}\license.rtf"
  File "${DIR_BIN}\whatsnew.html"
  !ifdef ICON_FILE
    File "${DIR_IMAGES}\${ICON_FILE}"
  !endif

  !ifdef APP_HELP_FILE
    SetOutPath "$INSTDIR\doc"
    File "doc\html\${APP_HELP_FILE}"
    SetOutPath "$INSTDIR\doc\img"
    File /r /x .svn "doc\html\img\*.*"
    CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Документация.lnk" "$INSTDIR\doc\${APP_HELP_FILE}"
  !endif

  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Лицензия.lnk" "$INSTDIR\license.rtf"
  CreateShortCut "$SMPROGRAMS\${START_MENU_ENTRY}\Что нового.lnk" "$INSTDIR\whatsnew.html"

  WriteUninstaller "$INSTDIR\uninst.exe";
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\visualdata.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  !ifdef ICON_FILE
    WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${ICON_FILE}"
  !endif
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Contact" "тел/факс.: 8 (863) 239-92-54"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
SectionEnd


;Function un.onUninstSuccess
;  HideWindow
;  MessageBox MB_ICONINFORMATION|MB_OK "Удаление программы $(^Name) было успешно завершено."
;FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Вы уверены в том, что желаете удалить $(^Name) и все компоненты программы?" IDYES +2
  Abort
FunctionEnd

Section Uninstall

  ; останавливаем простой сервер, если он запущен
  StrCpy $SRV_EXE "$INSTDIR\Server\server.exe"

  IfFileExists $SRV_EXE 0 if_skip1
    DetailPrint "Останавливается и удаляется сервер..."
    
    ; сперва пытаемся удалить его
    ClearErrors
    Delete $SRV_EXE
    IfErrors 0 if_skip1

    ; не удаляется - видимо запущен, посылаем окну команду завершиться
    FindWindow $0 "TMainForm" "VD Server"
    IntCmp $0 0 if_skip1
      SendMessage $0 ${WM_QUERYENDSESSION} 0 0
      
      StrCpy $1 0
      del_loop1:
        Sleep 1000
        ClearErrors
        Delete $SRV_EXE
        IntOp $1 $1 + 1
        IntCmp $1 60 0 skip1 0
          DetailPrint "Ошибка - не удалось остановить сервер. Остановите самостоятельно и запустите еще раз программу удаления."
          Abort
        skip1:
      IfErrors del_loop1
  if_skip1:

  ; надо остановить и удалить сервис
  IfFileExists "$INSTDIR\Server\serversvc.exe" 0 if_skip2
    DetailPrint "Останавливается и удаляется системная служба..."
    ExecWait '"$INSTDIR\Server\serversvc.exe" /name "vd_${APP_SHORT_NAME}" /stop /silent'
    ExecWait '"$INSTDIR\Server\serversvc.exe" /name "vd_${APP_SHORT_NAME}" /uninstall /silent'
    DetailPrint "Ждем ее завершения..."
    del_loop:
      Sleep 1000
      ClearErrors
      Delete "$INSTDIR\Server\serversvc.exe"
    IfErrors del_loop
    DetailPrint "Остановка завершена."
  if_skip2:

  Delete "$DESKTOP\${DESKTOP_LOCAL_LNK}"
  Delete "$DESKTOP\${DESKTOP_CLIENT_LNK}"
  RMDir /r "$INSTDIR\Doc"

  RMDir /r "$SMPROGRAMS\${START_MENU_ENTRY}"

  RMDir /r /REBOOTOK "$INSTDIR\Local\Meta"
  RMDir /r /REBOOTOK "$INSTDIR\Local\Errors"
  RMDir /r /REBOOTOK "$INSTDIR\Local\Reports"
  RMDir /r /REBOOTOK "$INSTDIR\Local\Forms"
  RMDir /r /REBOOTOK "$INSTDIR\Local\Templ"
  Delete /REBOOTOK "$INSTDIR\Local\*.*"
  RMDir /REBOOTOK "$INSTDIR\Local"

  RMDir /r /REBOOTOK "$INSTDIR\Client"
  RMDir /r /REBOOTOK "$INSTDIR\Demo"

  RMDir /r /REBOOTOK "$INSTDIR\Server\Meta"
  RMDir /r /REBOOTOK "$INSTDIR\Server\Errors"
  RMDir /r /REBOOTOK "$INSTDIR\Server\Reports"
  RMDir /r /REBOOTOK "$INSTDIR\Server\Forms"
  RMDir /r /REBOOTOK "$INSTDIR\Server\Templ"
  RMDir /r /REBOOTOK "$INSTDIR\Server\Client"
  Delete /REBOOTOK "$INSTDIR\Server\*.*"

  Delete /REBOOTOK "$INSTDIR\*.*"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
SectionEnd

