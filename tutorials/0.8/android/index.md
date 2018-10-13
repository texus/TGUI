---
layout: page
title: Android (experimental)
breadcrumb: android
---

Please note that support for android is still in an early stage. Building for android has so far only been tested on linux, altough it should be very similar on other systems.

You must use at least SFML 2.5.1 and Android NDK 13b or higher (tested up to NDK 17c).

### Requirements

You must have already build SFML for android and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI. This also means that the Android SDK and the Android NDK are thus already setup correctly.

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).

You will also need [gradle](https://gradle.org) to build the apk files.


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


### Running the example

To test if everything is working, you can build the example code.

Open the `TGUI/example/android` folder. Add a "local.properties" file with the following contents, specifying the android SDK and NDK paths:
```bash
sdk.dir=/path/to/android-sdk
ndk.dir=/path/to/android-ndk
```

Make sure to check the `abiFilters` in `app/build.gradle` and `APP_ABI` in `app/src/main/jni/Application.mk` inside the android example folder, they have to match the `CMAKE_ANDROID_ARCH_ABI` that was passed to cmake. The error that TGUI.hpp is not found can mean that this value is wrong. If you used a different value for `CMAKE_ANDROID_STL_TYPE` in cmake then you must also change `APP_STL` in `app/src/main/jni/Application.mk`.

Now you should be able to build project with gradle by running the following from the command line:
```bash
gradle build
```

If this results in an error stating `Could not open terminal for stdout: could not get termcap entry` then set the TERM variable to dumb (e.g. run `TERM=dumb gradle build` on linux).

If all goes well then you can now install the apk to a device (or a running simulator) by running
```bash
gradle installDebug
```


### Using TGUI

If you don't have much experience with android then your can just use the tgui demo code as a starting point. Below I will explain some settings that you need in order to use tgui if you have your own project set up.

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
