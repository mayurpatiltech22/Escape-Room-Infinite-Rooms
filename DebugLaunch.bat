@echo off
cd /d "%~dp0"
set PATH=%~dp0build-new;C:\msys64\ucrt64\bin;%PATH%
echo Running EscapeRoom...
"%~dp0build-new\EscapeRoom.exe" 2>&1
echo.
echo Exit code: %ERRORLEVEL%
echo.
pause
