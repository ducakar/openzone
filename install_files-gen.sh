#!/bin/sh

export LC_COLLATE=C

#
# liboz
#
echo 'string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" liboz_location_var )' > install_files.cmake
echo 'get_target_property( liboz_file oz ${liboz_location_var} )' >> install_files.cmake
echo >> install_files.cmake

echo 'if( OZ_INSTALL_LIBOZ )' >> install_files.cmake
echo '  install( FILES' >> install_files.cmake
echo '    ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp' >> install_files.cmake

ls src/oz/*.{hpp,cpp} | sed -e 's/^./    \0/' >> install_files.cmake

echo '    DESTINATION include/oz )' >> install_files.cmake
echo '  install( FILES ${liboz_file} DESTINATION lib )' >> install_files.cmake
echo 'endif()' >> install_files.cmake

echo >> install_files.cmake

#
# info files
#
echo 'if( WIN32 )' >> install_files.cmake
echo '  install( FILES' >> install_files.cmake
echo '    AUTHORS' >> install_files.cmake
echo '    COPYING' >> install_files.cmake
ls README* 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION . )' >> install_files.cmake
echo 'else()' >> install_files.cmake
echo '  install( FILES' >> install_files.cmake
echo '    AUTHORS' >> install_files.cmake
echo '    COPYING' >> install_files.cmake
ls README* 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/doc/openzone )' >> install_files.cmake
echo 'endif()' >> install_files.cmake

echo >> install_files.cmake

#
# oalinst, DLLs
#
echo 'if( WIN32 )' >> install_files.cmake
echo '  install( FILES' >> install_files.cmake
ls mingw32/{*.exe,*.dll} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION bin )' >> install_files.cmake
echo '  install( FILES' >> install_files.cmake
ls mingw32/*.bat 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION . )' >> install_files.cmake
echo 'endif()' >> install_files.cmake

echo >> install_files.cmake

#
# client
#
echo 'string( TOUPPER "LOCATION_${CMAKE_BUILD_TYPE}" client_location_var )' >> install_files.cmake
echo 'get_target_property( client_file openzone ${client_location_var} )' >> install_files.cmake
echo >> install_files.cmake

echo 'if( OZ_INSTALL_CLIENT )' >> install_files.cmake
echo '  install( FILES ${client_file}' >> install_files.cmake
echo '    DESTINATION bin' >> install_files.cmake
echo '    PERMISSIONS' >> install_files.cmake
echo '    OWNER_READ OWNER_WRITE OWNER_EXECUTE' >> install_files.cmake
echo '    GROUP_READ GROUP_EXECUTE' >> install_files.cmake
echo '    WORLD_READ WORLD_EXECUTE )' >> install_files.cmake
echo 'endif()' >> install_files.cmake

echo >> install_files.cmake

#
# data
#
echo 'if( OZ_INSTALL_DATA )' >> install_files.cmake

#
# share/openzone/class
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/class/{*.rc,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/class )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/ui/font
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/ui/font/{*.ttf,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/ui/font )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/ui/cur
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/ui/cur/{*.ozcCur,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/ui/cur )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/ui/icon
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/ui/icon/{*.ozcTex,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/ui/icon )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/lua
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/lua/{*.luac,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/lua )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/bsp
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/bsp/{*.{ozBSP,ozcBSP},*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/bsp )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/bsp/tex
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/bsp/tex/{*.ozcTex,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/bsp/tex )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/mdl
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/mdl/{*.{ozcSMM,ozcMD2},*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo "    DESTINATION share/openzone/mdl )" >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/name
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/name/{*.txt,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/name )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/snd
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/snd/{*.wav,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/snd )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/music
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/music/{*.oga,*README*,*COPYING*} 2> /dev/null | sed -e 's/^.*/    "\0"/' >> install_files.cmake
echo '    DESTINATION share/openzone/music )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/terra
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/terra/{*.{ozTerra,ozcTerra},*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/terra )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/caelum
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/caelum/{*.ozcCaelum,*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/caelum )' >> install_files.cmake
echo >> install_files.cmake

#
# share/openzone/glsl
#
echo '  install( FILES' >> install_files.cmake
ls share/openzone/glsl/{*.{glsl,vert,frag},*README*,*COPYING*} 2> /dev/null | sed -e 's/^./    \0/' >> install_files.cmake
echo '    DESTINATION share/openzone/glsl )' >> install_files.cmake

#
# share/locale
#
for langDir in share/locale/*; do
  echo >> install_files.cmake
  echo '  install( FILES' >> install_files.cmake
  echo "    $langDir/LC_MESSAGES/openzone.mo" >> install_files.cmake
  echo "    DESTINATION $langDir/LC_MESSAGES )" >> install_files.cmake
done

echo 'endif()' >> install_files.cmake
