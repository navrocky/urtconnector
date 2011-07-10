#!/bin/sh

# Vars
REV=`svnversion`
#
winfix() {
  sed s/"0.7.1 build .*"/"0.7.1 build $REV"/g distrib/windows/urtconnector.nsi > distrib/windows/urtconnector.nsi.1 || echo "Failed to nodify NSIS script" && exit 1
  sed s/"0.7.1 build .*"/"0.7.1 build $REV"\"\)/g version > version.1 || echo "Failed to modify VERSION file"
  rm distrib/windows/urtconnector.nsi version || echo "Failed to remove old NSIS script or version file"
  mv distrib/windows/urtconnector.nsi.1 distrib/windows/urtconnector.nsi || echo "Failed to move new NSIS script"
  mv version.1 version || echo "Failed to move new VERSION file"
  echo "! WINDOWS VERSION FIXED"
}

linfix() {
  sed s/"0.7.1 build .*"/"0.7.1 build $REV"\"\)/g version > version.1
  rm version
  mv version.1 version
  echo "! LINUX VERSION FIXED"
}

case $1 in
  linux)linfix;;
  windows)winfix;;
esac