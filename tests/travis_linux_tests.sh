set -e

export SFML_ROOT=$HOME/SFML-2.2_LINUX
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/2.2.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-2.2
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
  cd ..
else
  echo 'Using cached SFML directory.'
fi

mkdir build_gcc-5
cd build_gcc-5
cmake -DCMAKE_BUILD_TYPE=Debug -DTGUI_BUILD_TESTS=TRUE -DTGUI_USE_GCOV=TRUE ..
make -j2
cd tests/
./tests 2>&1 | grep --invert-match "Failed to use the XRandR extension while trying to get the desktop video modes"
cd ../..
