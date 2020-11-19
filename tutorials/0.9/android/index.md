---
layout: page
title: Android
breadcrumb: android
---

### Requirements

The tutorial assumes that the Android SDK and Android NDK are already installed correctly. TGUI was tested up to NDK r18b, as SFML 2.5.1 didn't work out of the box with newer NDK versions (it failed to find the GLES library).

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).

The TGUI source code can be downloaded from the [download page](/download).

The SFML backend has to be used to run TGUI on Android, other backends are currently unsupported. SFML should have already been installed inside the NDK before building TGUI.


### Building the library

Create a new folder inside the downloaded TGUI folder (e.g. called "build-android-arm64-v8a"). Open a terminal inside this folder and run the following command (with optionally a few changes as explained below).
```bash
cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=/path/to/ndk -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_BUILD_TYPE=Debug ..
```

**CMAKE_ANDROID_NDK** has to be the path to the NDK (which will end in something like "ndk/18.1.5063045" if you installed the NDK using the sdkmanager).

**CMAKE_ANDROID_ARCH_ABI** specifies the architecture to use. To use TGUI on real hardware you will need `arm64-v8a` (or `armeabi-v7a` if you still want to support 32-bit devices). To run TGUI on a simulator you will likely need `x86` (32-bit) or `x86_64` (64-bit), depending on which simulator you installed. The architecture has to match the SFML libraries, so you may be limited to x86 and armeabi-v7a as SFML 2.5.1 (latest version at time of writing) doesn't contain 64-bit extlibs and thus doesn't support arm64-v8a or x86_64 out of the box.

**CMAKE_BUILD_TYPE** chooses whether you want to build a Release or Debug library. The example code currently requires Debug libraries (due to hardcoded values).

On Windows you might also have to pass your [generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) to the cmake command with the -G parameter. On Linux and macOS, "Unix Makefiles" will be used by default.

Depending on the generator, CMake will have created a build file or project. For "Unix Makefiles" you use the following commands in the terminal to build the library and install it to the NDK folder. The "-j2" after the make command means it uses 2 threads, if you have more cores you can increase the number to speed up the build.
```bash
make -j2
make install
```


### Running the example

To test if everything is working, you can build the example code.

Open the `TGUI/examples/android` folder. Add a "local.properties" file with the following contents, specifying the android SDK and NDK paths:
```bash
sdk.dir=/path/to/android-sdk
ndk.dir=/path/to/android-ndk
```

Make sure to check the `abiFilters` in `app/build.gradle` and `APP_ABI` in `app/src/main/jni/Application.mk` inside the android example folder, they have to match the `CMAKE_ANDROID_ARCH_ABI` that was passed to cmake. The error that TGUI.hpp is not found can mean that this value is wrong.

Now you should be able to build project with gradle by running the following from the command line in the `TGUI/examples/android` directory:
```bash
./gradlew build
```

If this results in an error stating `Could not open terminal for stdout: could not get termcap entry` then set the TERM variable to dumb (e.g. run `TERM=dumb ./gradlew build` on Linux).

If all goes well then you can now install the apk to a device (or a running simulator) by running
```bash
./gradlew installDebug
```


### Using TGUI

In the Android.mk file you must add the tgui library
```bash
LOCAL_SHARED_LIBRARIES += tgui
```

You also have to import TGUI in that file. Importing SFML is not needed as TGUI will already do that for you.
```bash
$(call import-module, third_party/tgui)
```

In Application.mk you must enable exceptions and RTTI, add the tgui-activity module and make sure you use c++14 or higher.
```bash
APP_CPPFLAGS += -fexceptions -frtti -std=c++14
```
```bash
APP_MODULES := sfml-activity tgui-activity your-program
```

Finally you have to load the tgui-activity so that it can load the tgui library. You do that by having the following in your AndroidManifest.xml file (inside the activity element). The order is important, you must load SFML before TGUI. The `your-program` name should match the LOCAL_MODULE value in Android.mk.
```bash
<meta-data android:name="android.app.lib_name" android:value="sfml-activity" />
<meta-data android:name="sfml.app.lib_name" android:value="tgui-activity" />
<meta-data android:name="tgui.app.lib_name" android:value="your-program" />
```

That's it. You should now know enough to use tgui in your project.
