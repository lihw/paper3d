paper3d
=======

A cross-platform (iOS, Android and win32) rendering engine, best for creating Android live wallpapers.

For more information, please visit [http://lihw.github.io/paper3d](http://lihw.github.io/paper3d)

#Platform
Windows 7 32&64 bit, iOS 7.0, Android 4.1 and above.

#Developement requirements
The preferable develpoment platform is Windows 7 64bit with a discrete video card. The MacOS X 10.9 can also be used.

|               | Win32         | Andoird      |    iOS   |
| ------------- | ------------- |--------------|----------|
| Windows 7     | YES           |  Yes         |   No
| OSX           | No            | Not supported|   Yes    |


# Installation
Use either `git clone https://github.com/lihw/paper3d.git <dir>;git checkout 0.91a` or download the zip file at [https://github.com/lihw/paper3d/archive/0.91a.zip](https://github.com/lihw/paper3d/archive/0.91a.zip), and then uncompress the zip file to any dir you want.

Make sure [Tech](http://lihw.github.io/tech) is installed on your development platform.

## Windows 7
Run install.bat **as administrator**

## OSX
Run install.sh and *reboot the system*

# Build
## Windows
### Windows libraries
Open %FIPAPER3D%\paper3d\configs\msvc2012\paper3d.sln in MSVC2012, choose **Debug** profile and build. When finished, build the **Release** profile.

### Andorid libraries
Open %FIPAPER3D%\paper3d\configs\android\ in dos window, and enter `build.bat debug`. When finished, enter `build.bat release`.

## OSX
### iOS libraries
Open ${FIPAPER3D}/configs/ios/Paper3D.xcodeproj, select **buildall** as active scheme and build for both Debug and Release configurations respectively.


#Test

## Windows
1. Open %FIPAPER3D%\examples\hellopaper3d\configs\msvc2012\hellopaper3d.sln in MSVC2012, choose **Debug** profile, build and run.

## OSX
1. Open ${FIPAPER3D}/examples/loadscene3d/configs/ios/loadscene.xcodeproj, select **buildall** as active scheme and build for run.

# Create a wallpaper APK from FBX
Paper3D provides a very convenient way to create a wallpaper APK directly from an Autodesk FBX file. It takes only two steps.
step 1, in the dos window, enter `%FIPAPER3D%\tools\bin\msvc2012\pmake.exe some.fbx`, the command will create a project named after the fbx's filename.
step 2, enter the project created by pmake, in the application\configs\android, enter `build.bat debug`. It will then create the apk in the application\bin\android.

