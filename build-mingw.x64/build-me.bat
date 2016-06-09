@setlocal
@set TMPDIR=D:\mingw-w64\mingw64\bin
@if NOT EXIST %TMPDIR%\nul goto NODIR
@set TMPINS=..\..\software
@set TMPSRC=..
@if NOT EXIST %TMPSRC%\CMakeLists.txt goto NOCM
@set TMPLOG=bldlog-1.txt

@set TMPOPTS=
@REM set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX=%TMPINS%
@REM set TMPOPTS=%TMPOPTS% -DCMAKE_PREFIX_PATH:PATH=%TMP3RD%


@echo begin %DATE% %TIME% > %TMPLOG%

@set PATH=%TMPDIR%;%PATH%
@echo Added %TMPDIR% to the path >> %TMPLOG%

@echo Doing: 'cmake %TMPSRC% -G "MinGW Makefiles" %TMPOPTS%'
@echo Doing: 'cmake %TMPSRC% -G "MinGW Makefiles" %TMPOPTS%' >> %TMPLOG%
@cmake %TMPSRC% -G "MinGW Makefiles" %TMPOPTS% > %TMPLOG% 2>&1
@REM cmake ..\.. -G "Eclipse CDT4 - MinGW Makefiles"
@if ERRORLEVEL 1 goto Err1

@echo Doing 'mingw32-make' to %TMPLOG%
@echo Doing 'mingw32-make' >> %TMPLOG%
@mingw32-make >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto Err2

@echo.
@echo Appears a successful build...
@echo.
@goto END

:NOCM
@echo Can NOT locate '%TMPSRC%\CMakeLists.txt'! *** FIX ME ***
@goto ISERR

:Err1
@echo CMake configure and generation ERROR!
@goto ISERR

:Err2
@echo mongw32-make ERROR!
@goto ISERR

:NODIR
@echo Can NOT locate %TMPDIR%! *** FIX ME ***
@goto ISERR

:ISERR
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
