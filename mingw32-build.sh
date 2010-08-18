. mingw32-vars.sh
cd $PREFIX/build

echo
echo '*** Building zlib ***'
echo

tar zxf zlib-1.2.5.tar.gz
cd zlib-1.2.5
make -f win32/Makefile.gcc -j2 install
cd ..

# tar zxf libiconv-1.13.1.tar.gz
# cd libiconv-1.13.1
# ./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
# cd ..
#
# tar zxf libxml2-2.7.6.tar.gz
# cd libxml2-2.7.6
# ./configure --host=i486-mingw32 --prefix=$prefix --without-python && make -j2 install
# cd ..

echo
echo '*** Building libpng ***'
echo

tar Jxf libpng-1.4.3.tar.xz
cd libpng-1.4.3
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building libjpeg ***'
echo

tar zxf jpegsrc.v8b.tar.gz
cd jpeg-8b
./configure --host=i486-mingw32 --prefix=$prefix && make -j2
cd ..

echo
echo '*** Building libogg ***'
echo

tar zxf libogg-1.2.0.tar.gz
cd libogg-1.2.0
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building libvorbis ***'
echo

tar jxf libvorbis-1.3.1.tar.bz2
cd libvorbis-1.3.1
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building freetype ***'
echo

tar zxf freetype-2.4.2.tar.gz
cd freetype-2.4.2
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building lua ***'
echo

tar zxf lua-5.1.4.tar.gz
cd lua-5.1.4
sed -i -e "s/RANLIB=strip/RANLIB=$STRIP/" src/Makefile
make -j2 INSTALL_TOP=$prefix PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="$RANLIB" TO_BIN="lua.exe luac.exe"
make -j2 INSTALL_TOP=$prefix PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="$RANLIB" TO_BIN="lua.exe luac.exe" install
cd ..

echo
echo '*** Building SDL ***'
echo

tar zxf SDL-1.2.14.tar.gz
cd SDL-1.2.14
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building SDL_image ***'
echo

tar zxf SDL_image-1.2.10.tar.gz
cd SDL_image-1.2.10
CPPFLAGS="-I$PREFIX/include/SDL" ./configure --host=$triplet --prefix=$prefix && make -j2 LIBS="-lmingw32 -lSDLmain -lSDL" install
cd ..

echo
echo '*** Building SDL_ttf ***'
echo

tar zxf SDL_ttf-2.0.10.tar.gz
cd SDL_ttf-2.0.10
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

echo
echo '*** Building SDL_net ***'
echo

tar zxf SDL_net-1.2.7.tar.gz
cd SDL_net-1.2.7
./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
cd ..

# tar jxf openal-soft-1.8.466.bz2
# cd openal-soft-1.8.466
# echo '*************************************************'
# echo '1. set toolchain openzone/Toolchain-mingw32.cmake'
# echo '2. set PREFIX to ""'
# echo '*************************************************'
# cmake-gui .
# make -j2 DESTDIR=$PREFIX install
# cd ..
#
# tar zxf ../freealut-1.1.0.tar.gz
# cd freealut-1.1.0
# ./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 LIBS="-lOpenAL32" install
# cd ..
