#!/bin/sh
#
# package.sh {src | data | datasrc | boundle}
#
# One of the following commands must be given:
#
# - `src`: Create source archive `openzone-src-<version>.tar.xz`.
# - `data`: Create compiled data archive `openzone-data-<version>.tar.xz`. All data packages found
#   in `share/openzone` directory are included.
# - `datasrc`: Create source data archive `openzone-data-src-<version>.tar.xz`. All source data
#   packages found in `data` directory are included.
# - `boundle`: Create a ZIP archive that includes Linux-x86_64, Linux-i686 and Windows-i686
#   standalone builds and compiled game data packages found in `share/openzone`.
#

platforms=( Linux-x86_64-GCC Linux-i686-GCC Windows-i686 )

eval `egrep '^version=' ./autogen.sh`

files=`git ls-files | egrep -v '^data/'`
files="$files share/applications share/pixmaps"

case $1 in
  src)
    echo "Packing openzone-src-$version.tar.xz"
    tar Jcf openzone-src-$version.tar.xz --xform "s|^|openzone-$version/|" $files
    ;;
  data)
    echo "Packing openzone-data-$version.tar.xz"
    tar Jcf openzone-data-$version.tar.xz --xform "s|^|openzone-$version/|" share/openzone/*.zip
    ;;
  datasrc)
    echo "Packing openzone-data-src-$version.tar.xz"
    tar Jcf openzone-data-src-$version.tar.xz --xform "s|^|openzone-$version/|" \
        --exclude=DISABLED data
    ;;
  boundle)
    echo "Packing multi-platform OpenZone-$version.zip boundle"
    echo "TODO"
    ;;
  *)
    echo "Usage: $0 {src | data | datasrc | boundle}"
    ;;
esac
