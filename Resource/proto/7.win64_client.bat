@echo off

for /f "delims=" %%a in ('dir /b/a-d/oN *.proto') do 3.protoc.exe -I=./ --cpp_out=./client %%a
