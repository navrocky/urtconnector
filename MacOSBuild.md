# Some kind of introduction #
In current state urtconnector must be build on target system, cuz there are still some work in sources to automate this process.

This How-To will not help you with building of fully functional version of urtconnector ATM, we need to polish our code before.

# Preparations #
  * Install boost libraries using any port-like system - Fink, macports, etc.
  * Download and install Qt development files: http://qt.nokia.com/products/platform/qt-for-mac/
  * Download and install CMake: http://www.cmake.org/cmake/resources/software.html
  * Install qstat with any port-like system - Fink, macports, etc. (_not useful ATM, gathering server info don't work_)
  * Download and install Xcode

This How-To definetly suitable for MacOS X 10.6, but also must be suitable for 10.5 and 10.7 versions.

# Compiling from source #
After installing boost and qt you need to checkout fresh code: http://code.google.com/p/urtconnector/source/checkout

Using Terminal go to directory with urtconnector source and move in src directory, like this:

> ` $ cd ~/urtconnector-read-only/src `

Then execute following command:

> ` $ cmake -G Xcode -DQT_QMAKE_EXECUTABLE=$qtpath/Desktop/Qt/$qtver/gcc/bin/qmake ../ `

Where:
  * $qtpath - path where qt was installed (e.g. /Users/User/QtSDK)
  * $qtver - QT version (e.g. 474)

After that in urtconnector-read-only/src you can find urtconnector.xcodeproj. Open it in Xcode and compile.

# ToDo or What Has To Be Done #
  * Bundling
  * qstat in bundle (qstat in port-like systems is old)
  * Default settings for MacOS. Current default settings useful for Linux and BSD, not MacOS.
  * ...Maybe more

---

And here is a screenshot of urtconnector running on MacOS X 10.6.8:
![http://ompldr.org/vYWdhcQ/%D0%A1%D0%BD%D0%B8%D0%BC%D0%BE%D0%BA%20%D1%8D%D0%BA%D1%80%D0%B0%D0%BD%D0%B0%202011-09-21%20%D0%B2%208.19.06.png](http://ompldr.org/vYWdhcQ/%D0%A1%D0%BD%D0%B8%D0%BC%D0%BE%D0%BA%20%D1%8D%D0%BA%D1%80%D0%B0%D0%BD%D0%B0%202011-09-21%20%D0%B2%208.19.06.png)