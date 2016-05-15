set -eo pipefail

export SFML_ROOT=$HOME/SFML-2.2_LINUX
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/2.2.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-2.2
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

mkdir build_$CXX
cd build_$CXX
cmake -DCMAKE_BUILD_TYPE=Debug -DTGUI_BUILD_TESTS=TRUE -DTGUI_USE_GCOV=TRUE ..
make -j2
cd tests/
./tests 2>&1 | grep --invert-match "Failed to use the XRandR extension while trying to get the desktop video modes"
cd ../..
