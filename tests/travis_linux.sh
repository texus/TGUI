set -e

export SFML_ROOT=$HOME/SFML_LINUX
export BUILD_FOLDER=build_gcc-$CXX

git clone --depth 1 https://github.com/SFML/SFML
cd SFML

if [[ ! -d "$SFML_ROOT/lib" || ! -f "$SFML_ROOT/revision_cache" || `git rev-parse HEAD` != `cat "$SFML_ROOT/revision_cache"` ]]; then
  if [[ ! -d "$SFML_ROOT/lib" ]]; then
    echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"
  else
    echo "$(tput setaf 3)Rebuilding SFML: updating version$(tput sgr 0)"
  fi

  # SFML has to be patched when using GCC 4.9 or GCC 5
  git apply ../tests/ubuntu_sfml_gcc_patch.diff

  git rev-parse HEAD > "$SFML_ROOT/revision_cache"
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT -DSFML_BUILD_AUDIO=FALSE -DSFML_BUILD_NETWORK=FALSE .
  make -j2
  make install
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi
cd ..

mkdir $BUILD_FOLDER
cd $BUILD_FOLDER
cmake -DCMAKE_BUILD_TYPE=Release -DTGUI_BUILD_EXAMPLES=TRUE -DTGUI_BUILD_GUI_BUILDER=TRUE -DTGUI_OPTIMIZE_SINGLE_BUILD=TRUE -DTGUI_OPTIMIZE_SINGLE_BUILD_THREADS=2 ..
make -j2
cd ..

# Test the TGUIConfig.cmake file
cd tests/cmake
cmake -DSFML_DIR=$SFML_ROOT -DTGUI_DIR=$TRAVIS_BUILD_DIR/$BUILD_FOLDER .
make
test -e TguiTest
cd ../..
