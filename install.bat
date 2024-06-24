@echo off

mkdir bin
cd bin
mkdir cmake
cd cmake

cmake ..\.. -G "MinGW Makefiles"
cmake --build . --target install -- -j 12
