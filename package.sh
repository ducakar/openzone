#!/bin/bash
#
# package.sh {src | data | datasrc | bondle}
#
# One of the following commands must be given:
#
# - `src`: Create source archive `openzone-src-<version>.tar.xz`.
# - `data`: Create compiled data archive `openzone-data-<version>.tar.xz`. All data packages found
#   in `share/openzone` directory are included.
# - `datasrc`: Create source data archive `openzone-datasrc-<version>.tar.xz`. All source data
#   packages found in `data` directory are included.
# - `bundle`: Create a 7zip archive that contains Linux-x86_64, Linux-i686 and Windows-i686
#   standalone builds and compiled game data packages found in `share/openzone`.
#

set -e

platforms=(
  Linux-x86_64
  Windows-x86_64
)

eval "$(grep -E '^version=' ./autogen.sh)"

files=$(git ls-files | grep -Ev '^data$')
files="$files share/applications share/pixmaps"

. etc/common.sh

case $1 in
src)
  echo "Packing openzone-src-$version.tar.xz"
  tar Jcvf "openzone-src-$version.tar.xz" --owner=0 --group=0 --xform "s|^|openzone-$version/|" \
    "${files[@]}"
  ;;
data)
  echo "Packing openzone-data-$version.tar.xz"
  tar Jcvf "openzone-data-$version.tar.xz" --owner=0 --group=0 --xform "s|^|openzone-$version/|" \
    share/openzone/*.zip
  ;;
datasrc)
  echo "Packing openzone-datasrc-$version.tar.xz"
  tar Jcvf "openzone-data-src-$version.tar.xz" --owner=0 --group=0 --exclude=DISABLED \
    --xform "s|^|openzone-$version/|" data
  ;;
bundle)
  echo "Packing multi-platform OpenZone-$version.zip bundle"

  mkdir -p build/bundle && cd build/bundle
  rm -rf "OpenZone-$version" "../../OpenZone-$version-bundle.zip"

  for platform in "${platforms[@]}"; do
    mkdir -p "$platform" && cd "$platform"

    header_msg "$platform"

    cmake \
      -G Ninja \
      -D CMAKE_TOOLCHAIN_FILE="../../../cmake/$platform.Toolchain.cmake" \
      -D CMAKE_BUILD_TYPE=Release \
      -D OZ_BUNDLE=ON \
      ../../..
    ninja
    cmake -DCMAKE_INSTALL_PREFIX="../OpenZone-$version" -P cmake_install.cmake

    cd ..
  done

  header_msg "Packaging ..."

  rm -rf "OpenZone-$version"/{include,lib}
  zip -9r "../../OpenZone-$version-bundle.zip" "OpenZone-$version"
  cd ../..
  rm -rf build/bundle

  ls -hl --color=always "OpenZone-$version-bundle.zip"
  ;;
*)
  echo "Usage: $0 {src | data | datasrc | bundle}"
  ;;
esac
