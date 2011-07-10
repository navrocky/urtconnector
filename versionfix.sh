#!/bin/sh

# Vars
REV=`svnversion`
#
winfix() {
  sed s/"0.7.1 build ..."/"0.7.1 build $REV"/g distrib/windows/urtconnector.nsi > distrib/windows/urtconnector.nsi.1
  sed s/"0.7.1 build ..."/"0.7.1 build $REV"/g version > version.1
  rm distrib/windows/urtconnector.nsi version
  mv distrib/windows/urtconnector.nsi.1 distrib/windows/urtconnector.nsi
  mv version.1 version
  echo "! WINDOWS VERSION FIXED"
}

linfix() {
  sed s/"0.7.1 build ..."/"0.7.1 build $REV"/g version > version.1
  rm version
  mv version.1 version
  echo "! LINUX VERSION FIXED"
}

case $1 in
  linux)linfix;;
  windows)winfix;;
esac