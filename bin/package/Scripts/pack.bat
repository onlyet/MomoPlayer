@echo off
set /a StartS=%time:~6,2%
set /a StartM=%time:~3,2%
echo ��ʼʱ�䣺%time%

"makensis.exe" "pack.nsi"

set /a EndS=%time:~6,2%
set /a EndM=%time:~3,2%
echo ����ʱ�䣺%time%
set /a  diffS_=%EndS%-%StartS%
set /a  diffM_=%EndM%-%StartM%
echo ��������ʱ�䣺  %diffM_%����%diffS_%��

pause