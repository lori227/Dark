@echo off
echo "copy start...."

set srcpath=conf_output
set destpath=..\..\..\ChessDoc\Server\

if not exist %srcpath% (echo "copy file not exist" && pause)

xcopy /y %srcpath% %destpath% /e
