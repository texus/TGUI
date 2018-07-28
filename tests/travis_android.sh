set -e

mkdir -p $ANDROID_NDK
wget -nv https://dl.google.com/android/repository/android-ndk-r17b-linux-x86_64.zip
unzip -o -q android-ndk-r17b-linux-x86_64.zip -d $HOME

git clone --depth 1 https://github.com/SFML/SFML
cd SFML

if [[ ! -d "$ANDROID_NDK/sources/sfml/lib" || ! -f "$ANDROID_NDK/sources/sfml/revision_cache" || `git rev-parse HEAD` != `cat "$ANDROID_NDK/sources/sfml/revision_cache"` ]]; then
  if [[ ! -d "$ANDROID_NDK/sources/sfml/lib" ]]; then
    echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"
  else
    echo "$(tput setaf 3)Rebuilding SFML: updating version$(tput sgr 0)"
  fi

  git rev-parse HEAD > "$ANDROID_NDK/sources/sfml/revision_cache"
  cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$ANDROID_NDK -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Debug -DSFML_BUILD_AUDIO=FALSE  -DSFML_BUILD_NETWORK=FALSE .
  make -j2
  make install
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi
cd ..

mkdir build-android
cd build-android
cmake -DTGUI_OPTIMIZE_SINGLE_BUILD=TRUE -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=/home/texus/Documents/android/android-ndk-r13b/ -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Debug ..
make -j2
make install
cd ../examples/android
android update project --name TGUI --path . --target android-23
$ANDROID_NDK/ndk-build
ant debug
