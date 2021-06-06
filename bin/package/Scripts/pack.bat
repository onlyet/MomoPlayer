@echo off
set /a StartS=%time:~6,2%
set /a StartM=%time:~3,2%
echo 开始时间：%time%

"makensis.exe" "pack.nsi"

set /a EndS=%time:~6,2%
set /a EndM=%time:~3,2%
echo 结束时间：%time%
set /a  diffS_=%EndS%-%StartS%
set /a  diffM_=%EndM%-%StartM%
echo 程序运行时间：  %diffM_%分钟%diffS_%秒

pause