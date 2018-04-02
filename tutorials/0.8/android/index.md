---
layout: page
title: Android (experimental)
breadcrumb: android
---

Please note that support for android is still in an early stage. Building for android has so far only been tested on linux, altough it should be very similar on other systems.

Using TGUI was only successfully tested with android-ndk 12b and with ANDROID_STL=c++_shared set in cmake (which is the default value).
<!-- At least SFML 2.5 and android-ndk 13b are needed in order to use TGUI on android. -->

<span style="color:red;">Android support is currently broken in latest SFML and TGUI versions, use [SFML 2.4.2](https://www.sfml-dev.org/download/sfml/2.4.2/) and [TGUI 0.8-dev-2018-03-24](https://github.com/texus/TGUI/tree/287927d0f672cfde3d6d09d49a651bc15fe89170) for now.</span>


### Requirements

You must have already build SFML for android and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI. This also means that the Android SDK and the Android NDK are thus already setup correctly. Especially make sure that the ANDROID_NDK variable is set so that the installed SFML files in it can be found automatically.
<!-- You must have already build SFML for android and verified that you can run your sfml app on your device or emulator. Only then should you proceed with installing TGUI. This also means that the Android SDK and the Android NDK are thus already setup correctly. -->

You will need to use CMake in order to build TGUI. You can download the latest version [here](https://www.cmake.org/download/).

If you create the .apk file through the command line like described below then you will also need [Apache Ant](https://ant.apache.org).


### Building the library

From inside the TGUI folder that you download execute the following. The ANDROID_ABI should be set to the ABI that you want to use, this will most likely be "armeabi-v7a", but you can also set it to "armeabi", "x86" or "mips". On windows you might also have to pass your [generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) to the cmake command with the -G parameter. On linux and mac "Unix Makefiles" will be used by default.
<!-- From inside the TGUI folder that you downloaded, execute the following. The CMAKE_ANDROID_ARCH_ABI variable should be set to the ABI that you want to use, this will most likely be "armeabi-v7a" for actual hardware. The targeted SDK version can be specified with CMAKE_SYSTEM_VERSION. On windows you might also have to pass your [generator](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html) to the cmake command with the -G parameter. On linux and mac "Unix Makefiles" will be used by default. -->
{% highlight bash %}
mkdir build-android-arm
cd build-android-arm
cmake -DANDROID_ABI=armeabi-v7a -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/android.toolchain.cmake ..
{% endhighlight %}
<!-- cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=/path/to/ndk -DCMAKE_ANDROID_STL_TYPE=c++_shared -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_SYSTEM_VERSION=21 .. -->

Make sure that it finds the android sfml libraries. If you see a message like "Found SFML 2.4.2 in /usr/local/include" then it is finding the ones for your own system instead. Remove the libraries from /usr/local/lib and the includes from /usr/local/include, delete the cmake cache and rebuild. After that it should find the ones in the android ndk.

Depending on the generator, CMake will have created a build file or project. For "Unix Makefiles" (default on linux and mac) you use the following code to build the library and install it to the ndk folder. The "-j2" after the make command means it uses 2 threads, if you have more cores you can increase the number to speed up the build.
{% highlight bash %}
make -j2
make install
{% endhighlight %}

If you get tons of linking errors after running make then double check that you are including and linking to the android sfml libraries and not the ones for the OS that you are running.


### Running the demo

To test if everything is working, you can build the example code.

If you have build TGUI with a different ABI (e.g. you used x86 instead of armeabi-v7a to run it in a simulator), then make sure to edit APP_ABI in jni/Application.mk inside the android example folder, otherwise you can get the error that TGUI.hpp is not found.

First you have to run ndk-build. You can do this by opening your console inside the TGUI/example/android folder and running the following.
{% highlight bash %}
$ANDROID_NDK/ndk-build
{% endhighlight %}

If this succeeds then you can build the apk. The target sets the API level to use (you need this version of the android SDK installed).
{% highlight bash %}
Android-SDK-Path/tools/android update project --name TGUI --path . --target android-23
ant debug
cp bin/TGUI-debug.apk TGUI-example.apk
{% endhighlight %}

You can now install the .apk file to your android device (or a running simulator) with adb. Note that the adb server might have to run as root to have permissions to access the android device.
{% highlight bash %}
adb install TGUI-example.apk
{% endhighlight %}


### Using TGUI

If you don't have much experience with android then your can just use the tgui demo code as a starting point. But below I will explain some settings that you need in order to use tgui if you have your own project set up.

In the Android.mk file you must add the tgui library
{% highlight bash %}
LOCAL_SHARED_LIBRARIES += tgui
{% endhighlight %}

You also have to import tgui in that file. Importing sfml is not needed as tgui will already do that for you.
{% highlight bash %}
$(call import-module,tgui)
{% endhighlight %}

In Application.mk you must enable exceptions and RTTI and add the tgui-activity module
{% highlight bash %}
APP_CPPFLAGS += -fexceptions -frtti
{% endhighlight %}
{% highlight bash %}
APP_MODULES := sfml-activity tgui-activity your-program
{% endhighlight %}

Finally you have to load the tgui-activity so that it can load the tgui library. You do that by having this in your AndroidManifest.xml file
{% highlight xml %}
<meta-data android:name="android.app.lib_name" android:value="sfml-activity" />
<meta-data android:name="sfml.app.lib_name" android:value="tgui-activity" />
<meta-data android:name="tgui.app.lib_name" android:value="your-program" />
{% endhighlight %}

That's it. You should now know enough to use tgui in your project.
