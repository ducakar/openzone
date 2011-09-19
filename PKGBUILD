# Maintainer: Davorin Učakar <davorin.ucakar@gmail.com>
pkgname=openzone-git
pkgver=20110408
pkgrel=1
pkgdesc="A simple cross-platform 3D engine"
url="http://github.com/ducakar/openzone/"
arch=('i686' 'x86_64')
license=('GPL3')
depends=('gcc-libs' 'lua' 'sdl_ttf' 'libgl' 'freealut' 'libvorbis')
makedepends=('git' 'cmake' 'gcc>=4.5' 'gettext' 'sdl_image' 'mesa')
source=('http://openzone.googlecode.com/files/openzone-0.1.2-Windows-i686.zip')
sha1sums=('2e499a84b61bf2963ce9e5651e52f4adb91ed77e')

_gitroot='git://github.com/ducakar/openzone.git'
_gitname='openzone'

build() {
  cd ${srcdir}

  msg "git checkout for ${_gitroot}"
  if [ -d ${_gitname} ]; then
    ( cd ${_gitname} && git pull origin )
  else
    git clone ${_gitroot}
  fi
  msg "git checkout done or server timeout"

  cp -Rn ${srcdir}/openzone-0.1.2-Windows-i686/share/* ${srcdir}/openzone/share

  cd ${srcdir}/openzone
  ./autogen.sh

  rm -Rf build
  mkdir build
  cd build
  cmake -D CMAKE_INSTALL_PREFIX=/usr -D CMAKE_BUILD_TYPE=RelWithDebInfo \
      ${srcdir}/openzone
  make || return 1
  make install DESTDIR=${pkgdir}
}
