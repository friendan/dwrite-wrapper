@echo off
setlocal

rem Permission check
net session > nul 2>&1
if errorlevel 1 set ERR_MSG=Permission denied. & goto ERROR


if "%PROCESSOR_ARCHITECTURE%" EQU "AMD64" (
  del "%ProgramFiles%\Internet Explorer\DWrite.dll"
  del "%ProgramFiles%\Internet Explorer\EasyHook64.dll"
  del "%ProgramFiles(x86)%\Internet Explorer\DWrite.dll"
  del "%ProgramFiles(x86)%\Internet Explorer\EasyHook32.dll"
) else (
  del "%ProgramFiles%\Internet Explorer\DWrite.dll"
  del "%ProgramFiles%\Internet Explorer\EasyHook32.dll"
)

echo Uninstall successfully
echo Please User registry delete
echo  ex) reg delete "HKCU\Software\DWrite Wrapper"

:ERROR
if defined ERR_MSG echo %ERR_MSG%

endlocal
pause
exit /b 0
