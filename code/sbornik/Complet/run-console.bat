@echo off
set jmeno=%1
echo %jmeno% >input
set vystup="outs\%jmeno:~1,-1%.wrl"
if EXIST %vystup% goto cached
chenom.exe DATA\cp1250\settings.ini >%vystup% < input
set chyba=%ERRORLEVEL%
if ERRORLEVEL 1 (
	del input
	del %vystup%
	pause
	@echo on
	exit /B %chyba%
)
:cached
del input
explorer %vystup%
rem %vystup%
rem del %vystup%
@echo on
