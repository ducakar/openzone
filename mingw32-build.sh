. mingw32-vars.sh
cd mingw32/build

tar jxf zlib-1.2.3.tar.bz2
cd zlib-1.2.3
AR="i486-mingw32-ar rc" ./configure --prefix=$PREFIX && make -j2 install
cd ..

# tar zxf libiconv-1.13.1.tar.gz
# cd libiconv-1.13.1
# ./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
# cd ..
#
# tar zxf libxml2-2.7.6.tar.gz
# cd libxml2-2.7.6
# ./configure --host=i486-mingw32 --prefix=$PREFIX --without-python && make -j2 install
# cd ..

tar jxf libpng-1.2.40.tar.bz2
cd libpng-1.2.40
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf jpegsrc.v6b.tar.gz
cd jpeg-6b
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2
cd ..

tar zxf libogg-1.1.4.tar.gz
cd libogg-1.1.4
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf libvorbis-1.2.3.tar.gz
cd libvorbis-1.2.3
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar jxf freetype-2.3.11.tar.bz2
cd freetype-2.3.11
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf SDL-1.2.14.tar.gz
cd SDL-1.2.14
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf SDL_image-1.2.8.tar.gz
cd SDL_image-1.2.8
CPPFLAGS="-I$PREFIX/include/SDL" ./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 LIBS="-lmingw32 -lSDLmain -lSDL" install
cd ..

tar zxf SDL_ttf-2.0.9.tar.gz
cd SDL_ttf-2.0.9
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf SDL_net-1.2.7.tar.gz
cd SDL_net-1.2.7
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 install
cd ..

tar zxf lua-5.1.4.tar.gz
cd lua-5.1.4
sed -i -e 's/RANLIB=strip/RANLIB=i486-mingw32-strip/' src/Makefile
make -j2 INSTALL_TOP=$PREFIX PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="i486-mingw32-ranlib" TO_BIN="lua.exe luac.exe"
make -j2 INSTALL_TOP=$PREFIX PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="i486-mingw32-ranlib" TO_BIN="lua.exe luac.exe" install
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
