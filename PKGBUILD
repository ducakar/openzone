# Maintainer: Davorin Učakar <davorin.ucakar@gmail.com>

pkgbase=openzone
pkgname=('liboz' 'openzone' 'openzone-tools' 'openzone-data')
pkgver=20110920
pkgrel=1
_dataver=0.1.3
url="http://github.com/ducakar/openzone/"
license=('GPL3')
arch=('i686' 'x86_64')
makedepends=('git' 'cmake' 'gcc>=4.5' 'gettext' 'libvorbis' 'lua' 'mesa' 'sdl_ttf' 'sdl_image')
source=("http://openzone.googlecode.com/files/openzone-data-src-${_dataver}.tar.xz")
md5sums=('7e3fee55da5582b853213a3e472b8548')

_gitroot='git://github.com/ducakar/openzone.git'
_gitname='openzone-src'

build() {
  cd ${srcdir}

  msg "git checkout for ${_gitroot}"
  if [ -d ${_gitname} ]; then
    ( cd ${_gitname} && git pull origin )
  else
    git clone ${_gitroot} ${_gitname}
  fi
  msg "git checkout done or server timeout"

  cd ${srcdir}/${_gitname}

  mkdir -p build && cd build

  cmake \
    -D CMAKE_INSTALL_PREFIX=/usr \
    -D CMAKE_BUILD_TYPE=RelWithDebInfo \
    ${srcdir}/${_gitname}

  make || return 1

  cp -Rf ${srcdir}/openzone/share/* ${srcdir}/${_gitname}/share

  ./src/tools/ozPrebuild ${srcdir}/${_gitname}/share/openzone
}

package_liboz() {
  pkgdesc='Base library used by OpenZone 3D engine'
  depends=('gcc-libs')

  cd ${srcdir}/${_gitname}/build

  cmake \
    -D OZ_INSTALL_LIBOZ=1 \
    -D OZ_INSTALL_OPENZONE=0 \
    -D OZ_INSTALL_TOOLS=0 \
    -D OZ_INSTALL_INFO=0 \
    -D OZ_INSTALL_DATA=0 \
    -D OZ_INSTALL_DATA_SRC=0 \
    -D OZ_INSTALL_STANDALONE=0 \
    ${srcdir}/${_gitname}

  make install DESTDIR=${pkgdir}
}

package_openzone() {
  pkgdesc='A simple cross-platform 3D engine'
  depends=('libgl' 'libvorbis' 'lua' 'openal' 'openzone-data' 'sdl_ttf')

  cd ${srcdir}/${_gitname}/build

  cmake \
    -D OZ_INSTALL_LIBOZ=0 \
    -D OZ_INSTALL_OPENZONE=1 \
    -D OZ_INSTALL_TOOLS=0 \
    -D OZ_INSTALL_INFO=1 \
    -D OZ_INSTALL_DATA=0 \
    -D OZ_INSTALL_DATA_SRC=0 \
    -D OZ_INSTALL_STANDALONE=0 \
    ${srcdir}/${_gitname}

  make install DESTDIR=${pkgdir}
}

package_openzone-tools() {
  pkgdesc='Tools for prebuilding data for OpenZone 3D engine'
  depends=('libgl' 'libvorbis' 'lua' 'openal' 'sdl_image' 'sdl_ttf')

  cd ${srcdir}/${_gitname}/build

  cmake \
    -D OZ_INSTALL_LIBOZ=0 \
    -D OZ_INSTALL_OPENZONE=0 \
    -D OZ_INSTALL_TOOLS=1 \
    -D OZ_INSTALL_INFO=0 \
    -D OZ_INSTALL_DATA=0 \
    -D OZ_INSTALL_DATA_SRC=0 \
    -D OZ_INSTALL_STANDALONE=0 \
    ${srcdir}/${_gitname}

  make install DESTDIR=${pkgdir}
}

package_openzone-data() {
  pkgdesc='Runtime data for OpenZone 3D engine'
  arch=('any')

  cd ${srcdir}/${_gitname}/build

  cmake \
    -D OZ_INSTALL_LIBOZ=0 \
    -D OZ_INSTALL_OPENZONE=0 \
    -D OZ_INSTALL_TOOLS=0 \
    -D OZ_INSTALL_INFO=0 \
    -D OZ_INSTALL_DATA=1 \
    -D OZ_INSTALL_DATA_SRC=0 \
    -D OZ_INSTALL_STANDALONE=0 \
    ${srcdir}/${_gitname}

  make install DESTDIR=${pkgdir}
}
