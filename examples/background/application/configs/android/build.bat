@ECHO OFF
IF "%1" == "" GOTO USAGE
IF "%1" == "debug" GOTO DEBUG
IF "%1" == "release" GOTO RELEASE
IF "%1" == "clean" GOTO CLEAN
:DEBUG
ECHO ********************************************
ECHO Start building .so in debug version
ECHO ********************************************
ECHO ndk-build USE_STL=0 ENABLE_LOGGING=1 OPENGL=ES2 NDK_DEBUG=1 > temp.bat
CALL temp.bat
ECHO ********************************************
ECHO Start building Android APK in debug version
ECHO ********************************************
ant debug
GOTO EXIT
:RELEASE
ECHO ********************************************
ECHO Start building .so in release version
ECHO ********************************************
ECHO ndk-build USE_STL=0 ENABLE_LOGGING=1 OPENGL=ES2 NDK_DEBUG=0 -j4 > temp.bat
CALL temp.bat
ECHO ********************************************
ECHO Start building Android APK in release version
ECHO ********************************************
ant release
GOTO EXIT
:CLEAN
del /f /q ..\..\bin\android\*.*
GOTO EXIT
:USAGE
ECHO Usage: build.bat {debug ^| release ^| clean}
:EXIT

