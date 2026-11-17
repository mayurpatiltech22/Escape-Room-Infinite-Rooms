@echo off
cd /d "%~dp0"
set PATH=%~dp0build-new;C:\msys64\ucrt64\bin;%PATH%
start "" "%~dp0build-new\EscapeRoom.exe"
