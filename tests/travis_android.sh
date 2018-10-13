set -e

NDK_DIR=$HOME/android-ndk
NDK_VER=r17c

wget -nv https://dl.google.com/android/repository/android-ndk-${NDK_VER}-linux-x86_64.zip
unzip -o -q android-ndk-${NDK_VER}-linux-x86_64.zip
mv android-ndk-${NDK_VER} $NDK_DIR

sudo apt purge cmake
wget -nv https://cmake.org/files/v3.12/cmake-3.12.0-Linux-x86_64.tar.gz
tar -xvf cmake-3.12.0-Linux-x86_64.tar.gz > /dev/null
sudo cp -r cmake-3.12.0-Linux-x86_64/bin /usr/
sudo cp -r cmake-3.12.0-Linux-x86_64/share /usr/

mkdir -p $HOME/sfml-cache
git clone --depth 1 https://github.com/SFML/SFML
cd SFML

if [[ ! -d "$HOME/sfml-cache/lib" || ! -f "$HOME/sfml-cache/revision_cache" || `git rev-parse HEAD` != `cat "$HOME/sfml-cache/revision_cache"` ]]; then
  if [[ ! -d "$HOME/sfml-cache/lib" ]]; then
    echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"
  else
    echo "$(tput setaf 3)Rebuilding SFML: updating version$(tput sgr 0)"
  fi

  git rev-parse HEAD > "$HOME/sfml-cache/revision_cache"
  cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$NDK_DIR -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ANDROID_API=19 .
  make -j2
  make install
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
  mkdir -p $NDK_DIR/sources/third_party/sfml
  cp -R $HOME/sfml-cache/* $NDK_DIR/sources/third_party/sfml/
fi
cd ..

mkdir build-android
cd build-android
cmake -DTGUI_OPTIMIZE_SINGLE_BUILD=TRUE -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$NDK_DIR -DCMAKE_ANDROID_ARCH_ABI=x86 -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Debug ..
make -j2
make install
cd ../examples/android
echo ndk.dir=$NDK_DIR >> local.properties
./gradlew build  # Gradle 4.1+ is required but Travis only has 4.0 right now, so use a wrapper instead of just running "gradle build"

rm -R $HOME/sfml-cache/*
mv $NDK_DIR/sources/third_party/sfml/* $HOME/sfml-cache/
