set -e

mkdir -p $ANDROID_NDK
wget -nv https://dl.google.com/android/repository/android-ndk-r12b-linux-x86_64.zip
unzip -o -q android-ndk-r12b-linux-x86_64.zip -d $HOME

if [[ ! -d $ANDROID_NDK/sources/sfml/lib ]]; then
    echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

    wget -nv https://github.com/SFML/SFML/archive/2.4.2.tar.gz -O SFML.tar.gz
    tar -xzf SFML.tar.gz
    cd SFML-2.4.2

    # Skip building audio, which required several more dependencies to be installed in order to build SFML
    # The SFML_BUILD_AUDIO cmake option did not exist yet in the SFML version we use here.
    sed -i "s/add_subdirectory(Audio)/#add_subdirectory(Audio)/g" src/SFML/CMakeLists.txt

    cmake -DANDROID_ABI=x86 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/android.toolchain.cmake .
    make -j2
    make install
    cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

mkdir build-android
cd build-android
cmake -DANDROID_ABI=x86 -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/android.toolchain.cmake -DTGUI_OPTIMIZE_SINGLE_BUILD=TRUE ..
make -j2
make install
cd ../examples/android
sed -i "s/armeabi-v7a/x86/g" jni/Application.mk
$ANDROID_NDK/ndk-build
android update project --name TGUI --path . --target android-23
ant debug
