pwd
version=$(cat jenkins/version.txt)
rm -Rf build
mkdir -p build
cd build
cmake -D BUILD_PYSWIG_WRAPPERS:BOOL=OFF -D BUILD_LUA_WRAPPERS:BOOL=OFF .. || exit 1
make || exit 1
tar -cjf deepcl-linux64-${version}.tar.bz2 --exclude=CMake* --exclude=CMakeFiles --exclude=cmake* --exclude=Makefile --exclude=*.png --exclude=*.dat *

