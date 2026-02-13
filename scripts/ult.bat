@echo off
setlocal enabledelayedexpansion

set FORCE=0
set MONITOR=0

:parse
if "%~1"=="" goto endparse

if "%~1"=="-f" set FORCE=1
if "%~1"=="-m" set MONITOR=1

shift
goto parse

:endparse

cd client\web || exit /b 1
call npm run build
if errorlevel 1 exit /b 1

cd ..\embedded || exit /b 1

if "%FORCE%"=="1" (
    call pio run -t erase
    if errorlevel 1 exit /b 1

    call pio run -t clean
    if errorlevel 1 exit /b 1
)

call pio run -t upload
if errorlevel 1 exit /b 1

call pio run -t uploadfs
if errorlevel 1 exit /b 1

if "%MONITOR%"=="1" (
    call pio device monitor
)

endlocal

cd ..\..