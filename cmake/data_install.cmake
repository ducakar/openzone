#
# info files
#
if( OZ_INSTALL_INFO )
  if( WIN32 OR OZ_INSTALL_STANDALONE )
    set( dir "." )
  else()
    set( dir "share/doc" )
  endif()

  file( GLOB files README.* )
  install( FILES
    AUTHORS
    COPYING
    README
    ${files}
    DESTINATION ${dir} )
endif()

#
# glsl
#
if( OZ_INSTALL_OPENZONE )
  file( GLOB files
    share/openzone/glsl/*.glsl
    share/openzone/glsl/*.vert
    share/openzone/glsl/*.frag
    share/openzone/glsl/*README*
    share/openzone/glsl/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/glsl )
endif()

#
# icons
#
if( OZ_INSTALL_ICONS )
  install( DIRECTORY share/applications share/icons DESTINATION share )
endif()

#
# oalinst, DLLs
#
if( WIN32 )
  if( OZ_INSTALL_OPENZONE )
    file( GLOB files support/mingw32-client/*.exe support/mingw32-client/*.dll )
    install( FILES ${files} DESTINATION bin )

    if( OZ_INSTALL_STANDALONE )
      install( FILES support/oalinst.exe support/openzone.bat DESTINATION . )
    endif()
  endif()

  if( OZ_INSTALL_TOOLS )
    file( GLOB files support/mingw32-tools/*.exe support/mingw32-tools/*.dll )
    install( FILES ${files} DESTINATION bin )

    if( OZ_INSTALL_STANDALONE )
      install( FILES support/prebuild.bat DESTINATION . )
    endif()
  endif()
else()
  if( OZ_INSTALL_OPENZONE AND OZ_INSTALL_STANDALONE )
    install( FILES support/openzone.sh PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      DESTINATION . )
  endif()

  if( OZ_INSTALL_TOOLS AND OZ_INSTALL_STANDALONE )
    install( FILES support/prebuild.sh PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      DESTINATION . )
  endif()
endif()

#
# prebuilt data
#
if( OZ_INSTALL_DATA )

  install( FILES share/openzone/README DESTINATION share/openzone )

  # bsp
  file( GLOB files
    share/openzone/bsp/*.ozBSP
    share/openzone/bsp/*.ozcBSP
    share/openzone/bsp/*README*
    share/openzone/bsp/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/bsp )

  file( GLOB files
    share/openzone/bsp/tex/*.ozcTex
    share/openzone/bsp/tex/*README*
    share/openzone/bsp/tex/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/bsp/tex )

  # caelum
  file( GLOB files
    share/openzone/caelum/*.ozcCaelum
    share/openzone/caelum/*README*
    share/openzone/caelum/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/caelum )

  # class
  file( GLOB files
    share/openzone/class/*.rc
    share/openzone/class/*README*
    share/openzone/class/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/class )

  # lua
  file( GLOB files
    share/openzone/lua/matrix/*.lua
    share/openzone/lua/matrix/*README*
    share/openzone/lua/matrix/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/matrix )

  file( GLOB files
    share/openzone/lua/nirvana/*.lua
    share/openzone/lua/nirvana/*README*
    share/openzone/lua/nirvana/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/nirvana )

  # mdl
  file( GLOB files
    share/openzone/mdl/*.ozcSMM
    share/openzone/mdl/*.ozcMD2
    share/openzone/mdl/*README*
    share/openzone/mdl/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/mdl )

  # music
  file( GLOB files
    share/openzone/music/*.oga
    share/openzone/music/*README*
    share/openzone/music/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/music )

  # name
  file( GLOB files
    share/openzone/name/*.txt
    share/openzone/name/*README*
    share/openzone/name/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/name )

  # snd
  file( GLOB files
    share/openzone/snd/*.wav
    share/openzone/snd/*README*
    share/openzone/snd/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/snd )

  # terra
  file( GLOB files
    share/openzone/terra/*.ozTerra
    share/openzone/terra/*.ozcTerra
    share/openzone/terra/*README*
    share/openzone/terra/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/terra )

  # ui
  file( GLOB files
    share/openzone/ui/cur/*.ozcCur
    share/openzone/ui/cur/*README*
    share/openzone/ui/cur/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/cur )

  file( GLOB files
    share/openzone/ui/font/*.ttf
    share/openzone/ui/font/*README*
    share/openzone/ui/font/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/font )

  file( GLOB files
    share/openzone/ui/icon/*.ozcTex
    share/openzone/ui/icon/*README*
    share/openzone/ui/icon/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/icon )

  # locale
  file( GLOB dirs share/locale/* )
  foreach( dir IN ITEMS ${dirs} )
    file( RELATIVE_PATH dir ${CMAKE_SOURCE_DIR} ${dir} )
    install( FILES ${dir}/LC_MESSAGES/openzone.mo DESTINATION ${dir}/LC_MESSAGES )
  endforeach()

endif()

#
# development data
#
if( OZ_INSTALL_DATA_SRC )

  install( FILES share/openzone/README DESTINATION share/openzone )

  # caelum
  file( GLOB files
    share/openzone/caelum/*.png
    share/openzone/caelum/*.jpg
    share/openzone/caelum/*.xcf
    share/openzone/caelum/*README*
    share/openzone/caelum/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/caelum )

  # class
  file( GLOB files
    share/openzone/class/*.rc
    share/openzone/class/*README*
    share/openzone/class/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/class )

  # data
  file( GLOB files
    share/openzone/data/maps/*.rc
    share/openzone/data/maps/*.bsp
    share/openzone/data/maps/*.map
    share/openzone/data/maps/*README*
    share/openzone/data/maps/*COPYING* )
  foreach( file IN ITEMS ${files} )
    if( ${file} MATCHES "\\.autosave\\.map" )
      list( REMOVE_ITEM files ${file} )
    endif()
  endforeach()
  install( FILES ${files} DESTINATION share/openzone/data/maps )

  install( DIRECTORY share/openzone/data/scripts DESTINATION share/openzone/data/scripts )

  file( GLOB files
    share/openzone/data/textures/oz/*.png
    share/openzone/data/textures/oz/*.jpg
    share/openzone/data/textures/oz/*README*
    share/openzone/data/textures/oz/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/data/textures/oz )

  # lua
  file( GLOB files
    share/openzone/lua/matrix/*.lua
    share/openzone/lua/matrix/*README*
    share/openzone/lua/matrix/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/matrix )

  file( GLOB files
    share/openzone/lua/nirvana/*.lua
    share/openzone/lua/nirvana/*README*
    share/openzone/lua/nirvana/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/nirvana )

  # music
  file( GLOB files
    share/openzone/music/*.oga
    share/openzone/music/*README*
    share/openzone/music/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/music )

  # name
  file( GLOB files
    share/openzone/name/*.txt
    share/openzone/name/*README*
    share/openzone/name/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/name )

  # snd
  file( GLOB files
    share/openzone/snd/*.wav
    share/openzone/snd/*README*
    share/openzone/snd/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/snd )

  # terra
  file( GLOB files
    share/openzone/terra/*.rc
    share/openzone/terra/*.png
    share/openzone/terra/*.jpg
    share/openzone/terra/*README*
    share/openzone/terra/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/terra )

  # netradiant
  install( DIRECTORY share/openzone/netradiant DESTINATION share/openzone )

  # mdl
  file( GLOB files share/openzone/mdl/* )
  foreach( file IN ITEMS ${files} )
    if( NOT EXISTS ${file}/config.rc )
      list( REMOVE_ITEM files ${file} )
    endif()
  endforeach()
  install( DIRECTORY ${files} DESTINATION share/openzone/mdl )

  # ui
  file( GLOB files
    share/openzone/ui/cur/*.in
    share/openzone/ui/cur/*.png
    share/openzone/ui/cur/*README*
    share/openzone/ui/cur/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/cur )

  file( GLOB files
    share/openzone/ui/font/*.ttf
    share/openzone/ui/font/*README*
    share/openzone/ui/font/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/font )

  file( GLOB files
    share/openzone/ui/icon/*.png
    share/openzone/ui/icon/*.xcf
    share/openzone/ui/icon/*README*
    share/openzone/ui/icon/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/icon )

  # locale
  file( GLOB dirs share/locale/* )
  foreach( dir IN ITEMS ${dirs} )
    file( RELATIVE_PATH dir ${CMAKE_SOURCE_DIR} ${dir} )
    install( FILES ${dir}/LC_MESSAGES/openzone.po ${dir}/LC_MESSAGES/openzone.mo
      DESTINATION ${dir}/LC_MESSAGES )
  endforeach()

  install( FILES openzone.pot DESTINATION . )

  # launcher, icon
  install( DIRECTORY share/applications share/icons DESTINATION share )

  # support files (DLLs, launcher scripts ...)
  install( DIRECTORY support DESTINATION . )

endif()
