mkdir release
cd release
cmake -G "MinGW Makefiles" ..
mingw32-make
cd ..
pause