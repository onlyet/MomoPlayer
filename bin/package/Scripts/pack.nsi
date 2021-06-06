; �ýű�ʹ�� HM VNISEdit �ű��༭���򵼲���

; ��װ�����ʼ���峣��
!define PRODUCT_NAME "MomoPlayer"
!define PRODUCT_PATHNAME "MomoPlayer"  #��װж�����õ���KEY
!define PRODUCT_VERSION "0.1.0"
#!define PRODUCT_PUBLISHER ""
#!define PRODUCT_WEB_SITE ""
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!define PROGRAM_NAME "${PRODUCT_NAME}.exe"

SetCompressor lzma

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------
!include "MUI.nsh"

; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_ICON "..\Icon\install2.ico"
!define MUI_UNICON "..\Icon\uninstall1.ico"

; ��ӭҳ��
#!insertmacro MUI_PAGE_WELCOME
; ���Э��ҳ��
#!insertmacro MUI_PAGE_LICENSE "..\License\license.txt"
; ��װĿ¼ѡ��ҳ��
!insertmacro MUI_PAGE_DIRECTORY
; ��װ����ҳ��
!insertmacro MUI_PAGE_INSTFILES
; ��װ���ҳ��
!insertmacro MUI_PAGE_FINISH

; ��װж�ع���ҳ��
!insertmacro MUI_UNPAGE_INSTFILES

; ��װ�����������������
!insertmacro MUI_LANGUAGE "SimpChinese"

; ��װԤ�ͷ��ļ�
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI �ִ����涨����� ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\Release\${PRODUCT_NAME}_Setup_V${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
ShowInstDetails show
ShowUnInstDetails show
BrandingText "${PRODUCT_NAME}��װ����"

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
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\ж��${PRODUCT_NAME}.lnk" "$INSTDIR\uninst.exe"
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
 *  �����ǰ�װ�����ж�ز���  *
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

#-- ���� NSIS �ű��༭�������� Function ���α�������� Section ����֮���д���Ա��ⰲװ�������δ��Ԥ֪�����⡣--#

# �Զ�ж�ؾɰ汾
Var UNINSTALL_PROG
Var OLD_VER
Var OLD_PATH

Function .onInit
  ;�������Ƿ�����
  FindProcDLL::FindProc ${PROGRAM_NAME}
   Pop $R0
   IntCmp $R0 1 0 no_run
   MessageBox MB_ICONSTOP "��װ�����⵽ ${PRODUCT_NAME} �������У���ر�֮���ٰ�װ��"
   Quit
   no_run:

  ClearErrors
  ReadRegStr $UNINSTALL_PROG "${PRODUCT_UNINST_ROOT_KEY}" "${PRODUCT_UNINST_KEY}" "UninstallString"
  IfErrors  done
 
  ReadRegStr $OLD_VER "${PRODUCT_UNINST_ROOT_KEY}" "${PRODUCT_UNINST_KEY}" "DisplayVersion"
  MessageBox MB_YESNOCANCEL|MB_ICONQUESTION \
    "��⵽�����Ѿ���װ�� ${PRODUCT_NAME} $OLD_VER��\
    $\n$\n�Ƿ���ж���Ѱ�װ�İ汾��" \
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
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "��ȷʵҪ��ȫ�Ƴ� $(^Name) ���������е������" IDYES +2
  Abort
  
  # �������Ƿ�����
  FindProcDLL::FindProc ${PROGRAM_NAME}
  Pop $R0
  IntCmp $R0 1 0 no_run_monitor
  MessageBox MB_ICONSTOP "ж�س����⵽ ${PRODUCT_NAME} �������У���ر�֮����ж�أ�"
  Quit
  no_run_monitor:
  
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  #MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) �ѳɹ��ش����ļ�����Ƴ���"
FunctionEnd
