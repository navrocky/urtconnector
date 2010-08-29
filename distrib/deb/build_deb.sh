#!/bin/bash
a=$(pwd)
cd ..
cd ..
b=$(pwd)
cd $a
mkdir -p debian
cd debian
mkdir -p usr
mkdir -p DEBIAN
cd $b
cmake -DCMAKE_INSTALL_PREFIX=$a/debian/usr
VERSION=`cat version | awk '{if (match($0, "URT_VERSION \"(.+)\"", arr)) print arr[1];}'`
make
make install/strip
cd $a
c=$(uname -m)
if [ "$c" = 'x86_64' ]; then
    ARCH='amd64'
    else
    ARCH='i386'
fi
echo "Version: $VERSION">>control
echo "Architecture: $ARCH">>control
cp control debian/DEBIAN
dpkg-deb --build debian/ .
rm -R -f debian
