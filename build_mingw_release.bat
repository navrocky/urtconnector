mkdir release
cd release
cmake -G "MinGW Makefiles" ..
mingw32-make VERBOSE=on
strip src\urtconnector.exe
cd ..
pause