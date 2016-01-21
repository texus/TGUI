set -e

export SFML_ROOT=$HOME/SFML_LINUX

git clone --depth 1 https://github.com/SFML/SFML

cd SFML
if [[ ! -d "$SFML_ROOT/lib" || ! -f "$SFML_ROOT/revision_cache" || `git rev-parse HEAD` != `cat "$SFML_ROOT/revision_cache"` ]]; then
  git rev-parse HEAD > "$SFML_ROOT/revision_cache"
  cmake -DCMAKE_INSTALL_PREFIX=$SFML_ROOT .
  make -j2
  make install
else
  echo 'Using cached SFML directory.'
fi
cd ..

mkdir build_gcc-4.7
cd build_gcc-4.7
cmake -DCMAKE_BUILD_TYPE=Release -DTGUI_BUILD_EXAMPLES=TRUE ..
make -j2
cd ..
