@echo off
setlocal
cd /D "%~dp0\..\"

:: -- Unpack Arguments --------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if "%~1"==""                     echo [default mode, assuming `main` build] && set main=1
if "%~1"=="release" if "%~2"=="" echo [default mode, assuming `main` build] && set main=1

:: --- Unpack Command Line Build Arguments ------------------------------------
set auto_compile_flags=

set cl_common=   /I..\src\ /nologo /FC /Z7
set cl_debug=    call cl /Od /Ob1 /DBUILD_DEBUG=1 %cl_common% %auto_compile_flags%
set cl_release=  call cl /O2 /DBUILD_DEBUG=0 %cl_common% %auto_compile_flags%
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
if "%main%"=="1" set didbuild=1 && %compile% ..\src\main.c %compile_link% %out%main.exe || exit /b 1
popd

:: --- Unset ------------------------------------------------------------------
for %%a in (%*) do set "%%a=0"
set compile=
set compile_link=
set out=
set msvc=
set debug=
set release=