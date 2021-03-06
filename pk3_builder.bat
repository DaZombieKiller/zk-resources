echo off
title PK3 Builder
cls

echo === Choosing Compile Mode ===
choice /c yn /n /m "Compile for Zandronum? [Y,N] "

if %ERRORLEVEL%==1 set ZANDRONUM_COMPILE=1
if %ERRORLEVEL%==2 set ZANDRONUM_COMPILE=0
set ZANDRONUM_COMPAT=0

echo.
echo === Choosing Project ===
echo Choose which project to compile
echo.
echo [0] CHASECAM
echo [1] FLASHLIGHT (Unavailable)
echo [2] FOOTSTEPS
echo [3] HLFLASHLIGHT (Unavailable)
echo [4] GRAVITYGUN (Unavailable)
echo.
choice /c 01234 /n /m "Project Number: "
echo.

if %ERRORLEVEL%==1 goto compile_chasecam
if %ERRORLEVEL%==2 exit
if %ERRORLEVEL%==3 goto compile_footsteps
if %ERRORLEVEL%==4 exit
if %ERRORLEVEL%==5 exit
exit

:compile_flashlight
set PROJECT_FOLDER=project_flashlight
set PROJECT_NAME=flashlight
set PROJECT_LIBNAME=f_light
goto build

:compile_chasecam
set PROJECT_FOLDER=project_chasecam
set PROJECT_NAME=chasecam
set PROJECT_LIBNAME=chasecam
goto build

:compile_footsteps
set PROJECT_FOLDER=project_footsteps
set PROJECT_NAME=footsteps
set PROJECT_LIBNAME=footstep
goto build

:compile_hlflashlight
set PROJECT_FOLDER=project_hlflashlight
set PROJECT_NAME=hlflashlight
set PROJECT_LIBNAME=hl_light
goto build

:compile_gravitygun
set PROJECT_FOLDER=project_gravitygun
set PROJECT_NAME=gravitygun
set PROJECT_LIBNAME=gravgun
goto build

:build
echo === Compiling Source Code ===
set CC=.\GDCC\gdcc-cc.exe
set AS=.\GDCC\gdcc-as.exe
set LD=.\GDCC\gdcc-ld.exe
if exist ".\%PROJECT_FOLDER%\source\bin" del ".\%PROJECT_FOLDER%\source\bin" /s /q >nul 2>&1
if not exist ".\%PROJECT_FOLDER%\source\bin" mkdir ".\%PROJECT_FOLDER%\source\bin"
if not exist ".\%PROJECT_FOLDER%\source\bin\libgdcc" mkdir ".\%PROJECT_FOLDER%\source\bin\libgdcc"
if not exist ".\%PROJECT_FOLDER%\pk3\acs" mkdir ".\%PROJECT_FOLDER%\pk3\acs"
if not exist ".\%PROJECT_FOLDER%\source\inc" mkdir ".\%PROJECT_FOLDER%\source\inc"
if not exist ".\%PROJECT_FOLDER%\source\src" mkdir ".\%PROJECT_FOLDER%\source\src"

echo Makelib...
.\gdcc\gdcc-makelib --bc-target ZDoom libGDCC -c -o ".\%PROJECT_FOLDER%\source\bin\libgdcc\libGDCC.ir"
.\gdcc\gdcc-makelib --bc-target ZDoom libc    -c -o ".\%PROJECT_FOLDER%\source\bin\libgdcc\libc.ir"

echo %PROJECT_LIBNAME%.ir
if %ZANDRONUM_COMPILE%==1 %CC% --bc-target ZDoom -DZAN_ACS ".\%PROJECT_FOLDER%\source\src\*.c" .\base\src\*.c -ibase\inc -i "%PROJECT_FOLDER%\source\inc" -c -o ".\%PROJECT_FOLDER%\source\bin\%PROJECT_LIBNAME%.ir"
if not %ZANDRONUM_COMPILE%==1 (
	if %ZANDRONUM_COMPAT%==1 %CC% --bc-target ZDoom -DZAN_COMPAT ".\%PROJECT_FOLDER%\source\src\*.c" .\base\src\*.c -ibase\inc -i "%PROJECT_FOLDER%\source\inc" -c -o ".\%PROJECT_FOLDER%\source\bin\%PROJECT_LIBNAME%.ir"
	if not %ZANDRONUM_COMPAT%==1 %CC% --bc-target ZDoom ".\%PROJECT_FOLDER%\source\src\*.c" .\base\src\*.c -ibase\inc -i "%PROJECT_FOLDER%\source\inc" -c -o ".\%PROJECT_FOLDER%\source\bin\%PROJECT_LIBNAME%.ir"
)
echo %PROJECT_LIBNAME%.o
%LD% --bc-target ZDoom ".\%PROJECT_FOLDER%\source\bin\%PROJECT_LIBNAME%.ir" ".\%PROJECT_FOLDER%\source\bin\*.ir" ".\%PROJECT_FOLDER%\source\bin\libgdcc\*.ir" -o ".\%PROJECT_FOLDER%\pk3\acs\%PROJECT_LIBNAME%.o"

echo Finished.
echo.

echo === Making PK3 ===
echo Creating "%PROJECT_NAME%.pk3"...
tools\7za.exe a -tzip "%PROJECT_NAME%.pk3" ".\%PROJECT_FOLDER%\pk3\*" >nul
echo Finished.
echo.

echo === Finished ===
echo You should now see a "%PROJECT_NAME%.pk3" file in %cd%
echo Press any key to exit . . .
pause >nul
