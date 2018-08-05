---
layout: page
title: Android (experimental)
breadcrumb: android
---

Please note that support for android is still in an early stage. Building for android has so far only been tested on linux, altough it should be very similar on other systems.

You must use the SFML version from github, SFML 2.5.0 is NOT recent enough. Android NDK 13b or higher is required.

### Requirements

You must have already build SFML for android and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI. This also means that the Android SDK and the Android NDK are thus already setup correctly.

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).

If you create the .apk file through the command line like described below then you will also need [Apache Ant](https://ant.apache.org).


### Building the library

From inside the TGUI folder that you downloaded, execute the following. The `CMAKE_ANDROID_ARCH_ABI` variable should be set to the ABI that you want to use, this will most likely be `armeabi-v7a` for actual hardware. The targeted SDK version can be specified with `CMAKE_SYSTEM_VERSION`. On windows you might also have to pass your [generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) to the cmake command with the -G parameter. On linux and mac "Unix Makefiles" will be used by default.
```bash
mkdir build-android-x86
cd build-android-x86
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=/path/to/ndk -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Debug ..
```

Only change the `CMAKE_ANDROID_NDK` and `CMAKE_ANDROID_ARCH_ABI` in the above command, you currently must use `c++_static` as STL and `Debug` as build type.

Depending on the generator, CMake will have created a build file or project. For "Unix Makefiles" (default on linux and mac) you use the following code to build the library and install it to the ndk folder. The "-j2" after the make command means it uses 2 threads, if you have more cores you can increase the number to speed up the build.
```bash
make -j2
make install
```


### Running the demo

To test if everything is working, you can build the example code.

First make sure to check the `APP_ABI` in `jni/Application.mk` inside the android example folder, it has to match the `CMAKE_ANDROID_ARCH_ABI` that was passed to cmake. The error that TGUI.hpp is not found can mean that this value is wrong. If you used a different value for `CMAKE_ANDROID_STL_TYPE` in cmake then you must also change `APP_STL` in this file.

Opening your console inside the `TGUI/example/android` folder. Setup the project once with the following command. The `name` parameter will be used for the filename of the apk while the `target` specifies the API level (you need this version of the android SDK installed). If the `android` command is not in your PATH then use the absolute path instead (the `android` command is located in the tools directory inside the android SDK).
```bash
android update project --name TGUI --path . --target android-23
```

You can now build the apk file with the following commands. You will need to rerun both of these commands each time you make a change to the code. If the android NDK is not in your PATH then you must provide the absolute path of `ndk-build` instead.
```bash
ndk-build
ant debug
```

You can now install the .apk file to your android device (or a running simulator) with adb. Note that the adb server might have to run as root to have permissions to access the android device. The `-r` parameter is only to allow replacing the app if it already existed.
```bash
adb install -r bin/TGUI-debug.apk
```


### Using TGUI

If you don't have much experience with android then your can just use the tgui demo code as a starting point. But below I will explain some settings that you need in order to use tgui if you have your own project set up.

In the Android.mk file you must add the tgui library
```bash
LOCAL_SHARED_LIBRARIES += tgui
```

You also have to import tgui in that file. Importing sfml is not needed as tgui will already do that for you.
```bash
$(call import-module,tgui)
```

In Application.mk you must enable exceptions and RTTI, add the tgui-activity module and make sure you use c++14 or higher.
```bash
APP_CPPFLAGS += -fexceptions -frtti -std=c++14
```
```bash
APP_MODULES := sfml-activity tgui-activity your-program
```

Finally you have to load the tgui-activity so that it can load the tgui library. You do that by having the following in your AndroidManifest.xml file. The order is important, you must load SFML before TGUI.
```bash
<meta-data android:name="android.app.lib_name" android:value="sfml-activity" />
<meta-data android:name="sfml.app.lib_name" android:value="tgui-activity" />
<meta-data android:name="tgui.app.lib_name" android:value="your-program" />
```

That's it. You should now know enough to use tgui in your project.
