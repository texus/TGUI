set -eo pipefail

export SFML_VERSION=2.4.0
export SFML_ROOT=$HOME/SFML-${SFML_VERSION}_LINUX
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/${SFML_VERSION}.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-${SFML_VERSION}
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

mkdir build_$CXX
cd build_$CXX
CXXFLAGS="-DTGUI_SKIP_DRAW_TESTS" cmake -DCMAKE_BUILD_TYPE=Debug -DTGUI_BUILD_TESTS=TRUE -DTGUI_USE_GCOV=TRUE ..
make -j2
cd tests/
valgrind ./tests
cd ../..
