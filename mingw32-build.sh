. mingw32-vars.sh
echo $prefix
cd $prefix/build

#echo
#echo '*** Building iconv ***'
#echo
#
#tar zxf libiconv-1.13.1.tar.gz
#cd libiconv-1.13.1
#./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
#cd ..

#echo
#echo '*** Building libogg ***'
#echo
#
#tar Jxf libogg-1.2.2.tar.xz
#cd libogg-1.2.2
#./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
#cd ..

#echo
#echo '*** Building libvorbis ***'
#echo
#
#tar jxf libvorbis-1.3.2.tar.bz2
#cd libvorbis-1.3.2
#./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
#cd ..

#echo
#echo '*** Building freetype ***'
#echo
#
#tar jxf freetype-2.4.4.tar.bz2
#cd freetype-2.4.4
#./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
#cd ..

#echo
#echo '*** Building lua ***'
#echo
#
#tar zxf lua-5.1.4.tar.gz
#cd lua-5.1.4
#sed -i -e "s/RANLIB=strip/RANLIB=$STRIP/" src/Makefile
#make -j2 INSTALL_TOP=$prefix PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="$RANLIB" TO_BIN="lua.exe luac.exe"
#make -j2 INSTALL_TOP=$prefix PLAT=mingw CC=$CC CFLAGS="$CFLAGS" AR="$AR rcu" RANLIB="$RANLIB" TO_BIN="lua.exe luac.exe" install
#cd ..

#echo
#echo '*** Building SDL ***'
#echo
#
#tar zxf SDL-1.2.14.tar.gz
#cd SDL-1.2.14
#./configure --host=i486-mingw32 --prefix=$prefix && make -j2 install
#cd ..

# echo
# echo '*** Building SDL_ttf ***'
# echo
#
# tar zxf SDL_ttf-2.0.10.tar.gz
# cd SDL_ttf-2.0.10
# ./configure --host=i486-mingw32 --prefix=$prefix --with-freetype-prefix=$prefix --with-sdl-prefix=$prefix && make -j2 install
# cd ..

# tar jxf openal-soft-1.8.466.bz2
# cd openal-soft-1.8.466
# echo '*************************************************'
# echo '1. set toolchain openzone/Toolchain-mingw32.cmake'
# echo '2. set PREFIX to ""'
# echo '*************************************************'
# cmake-gui .
# make -j2 DESTDIR=$PREFIX install
# cd ..

echo
echo '*** Building freealut ***'
echo

tar zxf freealut-1.1.0.tar.gz
cd freealut-1.1.0
./configure --host=i486-mingw32 --prefix=$PREFIX && make -j2 prefix=$prefix exec_prefix=$prefix LIBS="-lOpenAL32" DESTDIR=$prefix install
cd ..
