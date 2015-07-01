# Windows build HowTo #

  1. **Installing Qt SDK**:
    * Take them from here http://qt.nokia.com/downloads ;
    * And install it: qt-sdk-XXX.exe -> next -> next -> next -> etc.
  1. **Installing Boost libraries**:
    * Download the source code of the latest boost library from here http://www.boost.org/users/download/. For example, source file for windows can be named as "boost\_X\_XX\_X.zip";
    * Extract to any folder, for example, c:\boost;
    * Set up you environment variable BOOST\_ROOT to "c:\boost";
    * To build boost you have to move mingw folder from, usually, C:\Qt\xxxx.xx\mingw to C:\MinGW;
    * add "C:\Qt\xxxx.xx\qt\bin; C:\MinGW\bin" to your PATH environment variable. See "System Properties" dialog, "Additional" tab, and button "Environment variables";
    * you are ready to build boost now ) execute "c:\boost\bootstrap.bat mingw" for build bjam;
    * exec "c:\boost\bjam.exe --toolset=gcc" and sit back..
  1. **Installing SQLite3**:
    * Go to the SQLite3 site (http://www.sqlite.org/download.html) and download following packages:
      * sqlite-amalgamation-XXXXXX.zip
      * sqlite-dll-win32-x86-XXXXXX.zip
    * Then extract the contents of these packages in the following folder: c:\sqlite
  1. **Installing CMake**. Download and install CMake from http://www.cmake.org/cmake/resources/software.html.
  1. **Building UrTConnector**.
    * Take a sources of UrTConnector via svn or download simple zip-file from download section http://code.google.com/p/urtconnector/downloads/list ;
    * Extract them to favorite folder;
    * And run build\_mingw\_release.bat from the sources root;
    * Resulting binary file will be in a "release\src" folder.

# Windows build using Visual Studio HowTo #

  1. **Installing Qt SDK**:
> > Like in MinGW HowTo
  1. **Starting Visual Studio command line tools**:
    * start VS; Tools->Visual Studio Command Prompt
    * start in cmd:
      1. start cmd
      1. cd C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin
      1. vcvars32.bat
  1. **Installing Boost libraries**:
    * Download the source code of the latest boost library from here http://www.boost.org/users/download/. For example, source file for windows can be named as "boost\_X\_XX\_X.zip";
    * Extract to any folder, for example, c:\boost;
    * Set up you environment variable BOOST\_ROOT to "c:\boost";
    * Start Visual Studio command line tools
    * cd boost
    * you are ready to build boost now ) execute bootstrap.bat for build bjam;
    * exec bjam.exe and sit back...
    * read the latest output and set up you environment variable BOOST\_LIBRARYDIR to "c:\boost\stage\lib\";
# **Installing SQLite3**:
    * Go to the SQLite3 site (http://www.sqlite.org/download.html) and download following package:
    * sqlite-amalgamation-XXXXXX.zip
    * Then extract the contents of these packages in the following folder: c:\sqlite
    * Start Visual Studio command line tools
    * compile sqlite3.lib:
      1. cd c:\sqlite
      1. cl -c sqlite3.c
      1. lib sqlite3.obj -OUT:sqlite3.lib
    * set up you environment variable SQLITE3\_ROOT to "c:\sqlite
    * sqlite3.lib ready
  1. **Installing CMake**.
> > Like in MinGW HowTo
  1. **Building UrTConnector**.
    * Take a sources of UrTConnector via svn or download simple zip-file from download section http://code.google.com/p/urtconnector/downloads/list ;
    * Extract them to favorite folder;
    * Start Visual Studio command line tools
    * And run build\_vc\_release.bat from the sources root;
    * Resulting binary file will be in a "release\src" folder.