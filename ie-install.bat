@echo off
setlocal

rem Permission check
net session > nul 2>&1
if errorlevel 1 set ERR_MSG=Permission denied. & goto ERROR

cd  /d "%~dp0"
if "%PROCESSOR_ARCHITECTURE%" EQU "AMD64" (
  copy /B /Y DWrite64.dll "%ProgramFiles%\Internet Explorer\DWrite.dll" > nul
  copy /B /Y EasyHook64.dll "%ProgramFiles%\Internet Explorer\EasyHook64.dll" > nul
  copy /B /Y DWrite32.dll "%ProgramFiles(x86)%\Internet Explorer\DWrite.dll" > nul
  copy /B /Y EasyHook32.dll "%ProgramFiles(x86)%\Internet Explorer\EasyHook32.dll" > nul
) else (
  copy /B /Y DWrite32.dll "%ProgramFiles%\Internet Explorer\DWrite.dll" > nul
  copy /B /Y EasyHook32.dll "%ProgramFiles%\Internet Explorer\EasyHook32.dll" > nul
)

echo Install successfully
echo Please User registry import
echo  ex) reg import DWrite.reg

:ERROR
if defined ERR_MSG echo %ERR_MSG%

endlocal
pause
exit /b 0
