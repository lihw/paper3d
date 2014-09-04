@ECHO OFF
@setlocal enableextensions
@cd /d "%~dp0"
CLS
echo Administrative permissions required. Detecting permissions...

net session >nul 2>&1
IF %errorLevel% == 0 (GOTO CONFIRMED) ELSE (GOTO ERROR)
:CONFIRMED
ECHO Administrative permissions confirmed.
ECHO Setting environment variables.
if DEFINED FITECH ( 
@setx /m FIPAPER3D %CD%
ECHO Installiaton completed. 
) ELSE (
ECHO Need to install Tech first ^(github.com/lihw/tech^).
)
GOTO EXIT
:ERROR
ECHO Not an administrator. 
ECHO Please run this script as administrator.
:EXIT
pause>null
del /q /f null
