rem @echo off
set jmeno=%1
echo %jmeno% >input
set vystup="outs\%jmeno:~1,-1%.wrl"
if EXIST %vystup% goto cached

rem ------------------
rem necachovat modely:
rem del /F /Q "outs\*.wrl"
rem ------------------

chenom.exe DATA\cp1250\settings.ini -e >%vystup% 2>error.log < input
if ERRORLEVEL 1 (
	del input

rem ------------------------
rem podrobny chybovy vystup:
rem	echo ________________________________________________________________________________ >> error.log
	echo ================================================================================ >> error.log
rem	echo :                                                                                >> error.log
	type %vystup% >> error.log
rem ------------------------

	del %vystup%
	rem pause
	@echo on
	exit 3
)
:cached
	del input
@echo on
