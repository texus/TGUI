set -e
mkdir build

export SFML_ROOT=$HOME/SFML_OSX

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/2.2.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-2.2
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT -DCMAKE_INSTALL_FRAMEWORK_PREFIX=$SFML_ROOT/lib -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" -DSFML_BUILD_FRAMEWORKS=TRUE .
  make -j2
  make install
  cd ..
else
  echo 'Using cached SFML directory.'
fi

cd build
cmake -DCMAKE_OSX_ARCHITECTURES="i386;x86_64" -DTGUI_BUILD_FRAMEWORK=TRUE -DSFML_INCLUDE_DIR=$SFML_ROOT/lib/SFML.framework ..
make -j2
