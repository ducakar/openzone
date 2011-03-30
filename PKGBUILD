# Maintainer: Davorin Učakar <davorin.ucakar@gmail.com>
pkgname=openzone-git
pkgver=20110329
pkgrel=1
pkgdesc="A simple cross-platform 3D engine"
url="http://github.com/ducakar/openzone/"
arch=('i686' 'x86_64')
license=('GPL3')
depends=('libgl' 'freealut' 'libvorbis' 'lua' 'sdl_ttf')
makedepends=('git' 'cmake' 'mesa' 'gcc>=4.6')
source=('http://openzone.googlecode.com/files/OpenZone-0.1.1-Windows-i686-strip.zip')
sha1sums=('8fdc253d3af030d1ee6093b0c6a003b815515f42')

_gitroot='git://github.com/ducakar/openzone.git'
_gitname='openzone'

build() {
  cd ${srcdir}

  msg "git checkout for ${_gitroot}"
  if [ -d ${srcdir}/${_gitname} ]; then
    ( cd ${srcdir}/${_gitname} && git pull origin )
  else
    git clone ${_gitroot}
  fi
  msg "git checkout done or server timeout"

  cp -Rn OpenZone-0.1.1-Windows-i686/share openzone

  cd ${srcdir}/openzone
  ./autogen.sh
  rm -rf ${srcdir}/openzone/build
  mkdir ${srcdir}/openzone/build
  cd ${srcdir}/openzone/build
  cmake -D CMAKE_INSTALL_PREFIX=/usr -D CMAKE_BUILD_TYPE=RelWithDebInfo \
      -D OZ_BUILD_CLIENT=1 -D OZ_BUILD_SERVER=0 -D OZ_BUILD_TOOLS=0 \
      -D OZ_BUILD_TESTS=0 -D OZ_INSTALL_LIBOZ=1 -D OZ_INSTALL_CLIENT=1 \
      -D OZ_INSTALL_SERVER=0 -D OZ_INSTALL_DATA=1 \
      ${srcdir}/openzone
  make || return 1
  make install DESTDIR=${pkgdir}
}
