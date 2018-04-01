set -eo pipefail

export SFML_VERSION=2.4.0
export SFML_ROOT=$HOME/SFML-${SFML_VERSION}_LINUX
export BUILD_FOLDER=build_$CXX
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

if [[ ! -d "$SFML_ROOT/lib" ]]; then
  echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"

  wget -O SFML.tar.gz https://github.com/SFML/SFML/archive/${SFML_VERSION}.tar.gz
  tar -xzf SFML.tar.gz
  cd SFML-${SFML_VERSION}

  # Skip building audio, which required several more dependencies to be installed in order to build SFML
  # The SFML_BUILD_AUDIO cmake option did not exist yet in the SFML version we use here.
  sed -i "s/add_subdirectory(Audio)/#add_subdirectory(Audio)/g" src/SFML/CMakeLists.txt

  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
  cd ..
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi

mkdir $BUILD_FOLDER
cd $BUILD_FOLDER
CXXFLAGS="-fprofile-arcs -ftest-coverage -DTGUI_SKIP_DRAW_TESTS" cmake -DCMAKE_BUILD_TYPE=Debug -DTGUI_BUILD_TESTS=TRUE -DTGUI_OPTIMIZE_TESTS_SINGLE_BUILD=TRUE -DTGUI_OPTIMIZE_SINGLE_BUILD_THREADS=2 ..
make -j2
cd tests/
./tests
cd ../..
