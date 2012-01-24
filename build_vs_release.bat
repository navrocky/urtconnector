mkdir release
cd release
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
nmake
cd ..
pause