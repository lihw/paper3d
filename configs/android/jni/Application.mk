# Parameters:
#  OPENGL: The version of OpenGL, e.g., OPENGL=ES2
#  ENABLE_LOGGING: Enable the logging
#  USE_STL: use stl or not, e.g, USE_STL=0
#  NDK_DEBUG: debug version or release one, e.g., NDK_DEBUG=1

APP_PLATFORM := android-9

ifeq ($(USE_STL), 1)
APP_STL := stlport_shared
endif

APP_MODULES := paper3d

