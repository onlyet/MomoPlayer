; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

; 安装程序初始定义常量
!define PRODUCT_NAME "MomoPlayer"
!define PRODUCT_PATHNAME "MomoPlayer"  #安装卸载项用到的KEY
!define PRODUCT_VERSION "0.1.0"
#!define PRODUCT_PUBLISHER ""
#!define PRODUCT_WEB_SITE ""
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!define PROGRAM_NAME "${PRODUCT_NAME}.exe"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "..\Icon\install2.ico"
!define MUI_UNICON "..\Icon\uninstall1.ico"

; 欢迎页面
#!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
#!insertmacro MUI_PAGE_LICENSE "..\License\license.txt"
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\Release\${PRODUCT_NAME}_Setup_V${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "${PRODUCT_NAME}安装程序"

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File /r "..\FileToInstall\*.*"
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
SectionEnd

Section -AdditionalIcons
  #WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.exe"
  #CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\XXX.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  #WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  #WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PROGRAM_NAME}"
SectionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

# 自动卸载旧版本
Var UNINSTALL_PROG
Var OLD_VER
Var OLD_PATH

Function .onInit
  ;检测程序是否运行
  FindProcDLL::FindProc ${PROGRAM_NAME}
   Pop $R0
   IntCmp $R0 1 0 no_run
   MessageBox MB_ICONSTOP "安装程序检测到 ${PRODUCT_NAME} 正在运行，请关闭之后再安装！"
   Quit
   no_run:

  ClearErrors
  ReadRegStr $UNINSTALL_PROG "${PRODUCT_UNINST_ROOT_KEY}" "${PRODUCT_UNINST_KEY}" "UninstallString"
  IfErrors  done
 
  ReadRegStr $OLD_VER "${PRODUCT_UNINST_ROOT_KEY}" "${PRODUCT_UNINST_KEY}" "DisplayVersion"
  MessageBox MB_YESNOCANCEL|MB_ICONQUESTION \
    "检测到本机已经安装了 ${PRODUCT_NAME} $OLD_VER。\
    $\n$\n是否先卸载已安装的版本？" \
      /SD IDYES \
      IDYES uninstall1 \
      IDNO done
  Abort
 
uninstall1:
  StrCpy $OLD_PATH $UNINSTALL_PROG -10
  ExecWait '"$UNINSTALL_PROG" _?=$OLD_PATH' $0
  DetailPrint "uninst.exe returned $0"
 
done:
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
  Abort
  
  # 监测程序是否运行
  FindProcDLL::FindProc ${PROGRAM_NAME}
  Pop $R0
  IntCmp $R0 1 0 no_run_monitor
  MessageBox MB_ICONSTOP "卸载程序检测到 ${PRODUCT_NAME} 正在运行，请关闭之后再卸载！"
  Quit
  no_run_monitor:
  
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  #MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从您的计算机移除。"
FunctionEnd
