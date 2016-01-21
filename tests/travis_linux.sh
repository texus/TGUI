set -e

export SFML_ROOT=$HOME/SFML_LINUX

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

mkdir build_gcc-4.7
cd build_gcc-4.7
cmake -DCMAKE_BUILD_TYPE=Release -DTGUI_BUILD_EXAMPLES=TRUE ..
make -j2
cd ..
