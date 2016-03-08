set -eo pipefail

export SFML_ROOT=$HOME/SFML_LINUX
export DISPLAY=:99.0
sh -e /etc/init.d/xvfb start

git clone --depth 1 https://github.com/SFML/SFML

cd SFML
if [[ ! -d "$SFML_ROOT/lib" || ! -f "$SFML_ROOT/revision_cache" || `git rev-parse HEAD` != `cat "$SFML_ROOT/revision_cache"` ]]; then
  if [[ ! -d "$SFML_ROOT/lib" ]]; then
    echo "$(tput setaf 3)Rebuilding SFML: no cache available$(tput sgr 0)"
  else
    echo "$(tput setaf 3)Rebuilding SFML: updating version$(tput sgr 0)"
  fi

  git rev-parse HEAD > "$SFML_ROOT/revision_cache"
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
else
  echo "$(tput setaf 2)Using cached SFML directory$(tput sgr 0)"
fi
cd ..

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DTGUI_BUILD_TESTS=TRUE -DTGUI_USE_GCOV=TRUE ..
make -j2
cd tests/
./tests
cd ../..
