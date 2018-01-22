@setlocal
@set TMPBLDS=
@set TMPCNT=0
@set TMPDIR=build
@echo.
@if EXIST %TMPDIR%\nul (
@cd %TMPDIR%
@echo Build using default MSVC x86 in %TMPDIR%
@call bm
@if ERRORLEVEL 1 (
@echo Note ERROREXIT in %TMPDIR%
) else (
@set TMPBLDS=%TMPBLDS% %TMPDIR%
@set /A TMPCNT+=1
)
cd ..
) else (
@echo Note: %TMPDIR% does NOT EXIST
)


@set TMPDIR=build.x64
@echo.
@if EXIST %TMPDIR%\nul (
@cd %TMPDIR%
@echo Build using MSVC x64 in %TMPDIR%
@call bm
@if ERRORLEVEL 1 (
@echo Note ERROREXIT in %TMPDIR%
) else (
@set TMPBLDS=%TMPBLDS% %TMPDIR%
@set /A TMPCNT+=1
)
cd ..
) else (
@echo Note: %TMPDIR% does NOT EXIST
)

@set TMPDIR=build-mingw.x64
@echo.
@if EXIST %TMPDIR%\nul (
@cd %TMPDIR%
@echo Build using mingw.x64 in %TMPDIR%
@call bm
@if ERRORLEVEL 1 (
@echo Note ERROREXIT in %TMPDIR%
) else (
@set TMPBLDS=%TMPBLDS% %TMPDIR%
@set /A TMPCNT+=1
)
cd ..
) else (
@echo Note: %TMPDIR% does NOT EXIST
)

@set TMPDIR=build.vc10
@echo.
@if EXIST %TMPDIR%\nul (
@cd %TMPDIR%
@echo Build using MSVC10 x64 in %TMPDIR%
@call bm
@if ERRORLEVEL 1 (
@echo Note ERROREXIT in %TMPDIR%
) else (
@set TMPBLDS=%TMPBLDS% %TMPDIR%
@set /A TMPCNT+=1
)
cd ..
) else (
@echo Note: %TMPDIR% does NOT EXIST
)

@echo.
@echo Successful %TMPCNT% builds in %TMPBLDS%
@echo.

@REM eof
