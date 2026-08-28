@echo off
setlocal EnableExtensions

set "WDKROOT=C:\WinDDK\7600.16385.1"
if defined BASEDIR set "WDKROOT=%BASEDIR%"

set "SRC=comptelctrl.c"
set "RCFILE=comptelctrl.rc"
set "OBJ=comptelctrl.obj"
set "RES=comptelctrl.res"
set "EXE=%CD%\comptelctrl.exe"

echo.
echo ============================================================
echo CompatTelControl v1.1 - WDK 7.1 build with VERSIONINFO
echo ============================================================
echo WDK root : %WDKROOT%
echo Output   : %EXE%
echo.

if not exist "%SRC%" (
    echo ERROR: Cannot find %SRC%
    goto :fail
)

if not exist "%RCFILE%" (
    echo ERROR: Cannot find %RCFILE%
    goto :fail
)

set "PATH=%WDKROOT%\bin\x86\x86;%WDKROOT%\bin\x86;%PATH%"
set "INCLUDE=%WDKROOT%\inc\crt;%WDKROOT%\inc\api;%WDKROOT%\inc\ddk;%INCLUDE%"
set "LIB=%WDKROOT%\lib\crt\i386;%WDKROOT%\lib\win7\i386;%LIB%"

if exist "%OBJ%" del /f /q "%OBJ%"
if exist "%RES%" del /f /q "%RES%"
if exist "%EXE%" del /f /q "%EXE%"

echo [1/3] Compiling C source...
cl.exe /nologo /W4 /MT /GS- /c "%SRC%" /Fo"%OBJ%"
if errorlevel 1 goto :compile_fail

if not exist "%OBJ%" (
    echo ERROR: %OBJ% was not created.
    goto :fail
)

echo.
echo [2/3] Compiling Windows VERSIONINFO resource...
rc.exe /fo "%RES%" "%RCFILE%"
if errorlevel 1 goto :resource_fail

if not exist "%RES%" (
    echo ERROR: %RES% was not created.
    goto :fail
)

echo.
echo [3/3] Linking EXE + VERSIONINFO...
link.exe /NOLOGO ^
    /OUT:"%EXE%" ^
    /SUBSYSTEM:CONSOLE ^
    /MACHINE:X86 ^
    "%OBJ%" ^
    "%RES%" ^
    shell32.lib ^
    advapi32.lib ^
    kernel32.lib ^
    libcmt.lib ^
    oldnames.lib

if errorlevel 1 goto :link_fail

if not exist "%EXE%" (
    echo ERROR: Linker returned success but EXE was not created.
    goto :fail
)

echo.
echo ============================================================
echo BUILD REALLY SUCCEEDED
echo ============================================================
echo.
echo EXE created at:
echo   %EXE%
echo.
echo Version Information has been embedded.
echo Right-click EXE ^> Properties ^> Details to view it.
echo.
dir /a "%EXE%"
echo.
goto :eof

:compile_fail
echo.
echo ERROR: C compilation failed.
goto :fail

:resource_fail
echo.
echo ERROR: Resource compilation failed.
echo Please copy the [2/3] error message and send it back.
goto :fail

:link_fail
echo.
echo ERROR: Linker failed.
goto :fail

:fail
echo.
echo ============================================================
echo BUILD FAILED - NO EXE CREATED
echo ============================================================
echo.
exit /b 1
