#!/bin/bash
a=$(pwd)
cd ..
cd ..
b=$(pwd)
sh build_gcc_linux_release
cd $a
mkdir -p debian
cd debian
mkdir -p usr
mkdir -p DEBIAN
cd usr
mkdir -p local
cd local
mkdir bin
cd ..
mkdir share
cd share
mkdir icons
cp $b/release/src/urtconnector $a/debian/usr/local/bin/
cp $b/src/icons/urtconnector.png $a/debian/usr/share/icons/
cd $a
c=$(uname -m)
if [ "$c" = 'x86_64' ]; then
    ARCH='amd64'
    else
    ARCH='i386'
fi
echo "Architecture: $ARCH">>control
cp control debian/DEBIAN
dpkg-deb --build debian/ .
rm -R -f debian
