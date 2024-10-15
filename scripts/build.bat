@echo off
setlocal
cd /D "%~dp0\..\"

:: -- Unpack Arguments --------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if "%rebuild_shaders%"=="1"      echo [rebuilding shaders]
if "%~1"==""                     echo [default mode, assuming `main` build] && set main=1
if "%~1"=="release" if "%~2"=="" echo [default mode, assuming `main` build] && set main=1

if "%rebuild_shaders%"=="1" python .\scripts\build_shaders.py --src ".\src\opus\draw\shaders" --out=".\src\opus\draw\draw_shaders.h"

:: --- Unpack Command Line Build Arguments ------------------------------------
set compile_flags=/DLOG_DEBUG_ENABLED=0
set deps= ..\lib\freetype.lib ..\lib\glfw3dll.lib ..\lib\gl.obj opengl32.lib user32.lib

if "%asan%"=="1" set compile_flags=%compile_flags% -fsanitize=address && echo [asan enabled]

:: --- Compile Arguments ------------------------------------------------------
set cl_warnings= /W4 /WX /wd4201 /wd4456
set cl_common=   /I..\src\ /I..\src\opus\third_party\ /nologo /FC /Z7 %deps%
set cl_debug=    call cl /Od /Ob1 /DBUILD_DEBUG=1 %cl_warnings% %cl_common% %compile_flags%
set cl_release=  call cl /O2 /DBUILD_DEBUG=0 %cl_common% %compile_flags%
set cl_link=     /link /MANIFEST:EMBED /INCREMENTAL:NO /pdbaltpath:%%%%_PDB%%%%
set cl_out=      /out:

:: --- Compile/Link Line Definitions ------------------------------------------
set compile_debug=%cl_debug%
set compile_release=%cl_release%
set compile_link=%cl_link%
set out=%cl_out%

:: --- Choose Compile/Link Lines ----------------------------------------------
:: TODO: add clang compiler option
if "%debug%"=="1"   set compile=%compile_debug%
if "%release%"=="1" set compile=%compile_release% && echo hi

:: --- Prep Directories -------------------------------------------------------
if not exist dist mkdir dist

:: --- Build Everything (@build_targets) --------------------------------------
pushd dist
if "%main%"=="1" echo [building main.c] && set didbuild=1 && %compile% ..\src\main.c %compile_link% %out%main.exe || exit /b 1
popd
if %didbuild%==1 echo [build succeeded]

pushd dist
:: TODO: replace main with build target name
if "%run%"=="1" echo [running main.exe] && call main.exe
popd


:: --- Unset ------------------------------------------------------------------
for %%a in (%*) do set "%%a=0"
set compile=
set compile_link=
set out=
set msvc=
set debug=
set release=
set deps=

:: --- Warn On No Builds ------------------------------------------------------
if "%didbuild%"=="" (
  echo [WARNING] no valid build target specified; must use build target names as arguments to this script, like `build main`.
  exit /b 1
)
