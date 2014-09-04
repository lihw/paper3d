@ECHO OFF
IF "%1" == "" GOTO USAGE
IF "%1" == "debug" GOTO DEBUG
IF "%1" == "release" GOTO RELEASE
IF "%1" == "clean" GOTO CLEAN
:DEBUG
ndk-build USE_STL=0 ENABLE_LOGGING=1 OPENGL=ES2 NDK_DEBUG=1 -j4
:RELEASE
ndk-build USE_STL=0 ENABLE_LOGGING=1 OPENGL=ES2 NDK_DEBUG=0 -j4
:CLEAN
echo ndk-build OPENGL=ES2 NDK_DEBUG=0 -j4 clean > temp.bat
CALL temp.bat
echo ndk-build OPENGL=ES2 NDK_DEBUG=1 -j4 clean > temp.bat
CALL temp.bat
GOTO EXIT
:USAGE
ECHO Usage: build.bat {debug^|release^|clean}
:EXIT
