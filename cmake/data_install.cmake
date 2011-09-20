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
    DESTINATION ${dir}
    COMPONENT info )
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
  install( FILES ${files} DESTINATION share/openzone/glsl COMPONENT openzone )
endif()

#
# oalinst, DLLs
#
if( WIN32 )
  if( OZ_INSTALL_OPENZONE )
    file( GLOB files support/mingw32-client/*.exe support/mingw32-client/*.dll )
    install( FILES ${files} DESTINATION bin COMPONENT openzone )

    if( OZ_INSTALL_STANDALONE )
      install( FILES support/oalinst.exe support/openzone.bat DESTINATION . COMPONENT openzone )
    endif()
  endif()

  if( OZ_INTALL_TOOLS )
    file( GLOB files support/mingw32-client/*.exe support/mingw32-client/*.dll )
    install( FILES ${files} DESTINATION bin COMPONENT tools )

    if( OZ_INSTALL_STANDALONE )
      install( FILES support/prebuild.bat DESTINATION . COMPONENT tools )
    endif()
  endif()
else()
  if( OZ_INSTALL_OPENZONE AND OZ_INSTALL_STANDALONE )
    install( FILES support/openzone.sh PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      DESTINATION . COMPONENT openzone )
  endif()

  if( OZ_INSTALL_TOOLS AND OZ_INSTALL_STANDALONE )
    install( FILES support/prebuild.sh PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
      DESTINATION . COMPONENT tools )
  endif()
endif()

#
# prebuilt data
#
if( OZ_INSTALL_DATA )

  install( FILES share/openzone/README DESTINATION share/openzone COMPONENT data )

  # bsp
  file( GLOB files
    share/openzone/bsp/*.ozBSP
    share/openzone/bsp/*.ozcBSP
    share/openzone/bsp/*README*
    share/openzone/bsp/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/bsp COMPONENT data )

  file( GLOB files
    share/openzone/bsp/tex/*.ozcTex
    share/openzone/bsp/tex/*README*
    share/openzone/bsp/tex/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/bsp/tex COMPONENT data )

  # caelum
  file( GLOB files
    share/openzone/caelum/*.ozcCaelum
    share/openzone/caelum/*README*
    share/openzone/caelum/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/caelum COMPONENT data )

  # class
  file( GLOB files
    share/openzone/class/*.rc
    share/openzone/class/*README*
    share/openzone/class/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/class COMPONENT data )

  # lua
  file( GLOB files
    share/openzone/lua/matrix/*.lua
    share/openzone/lua/matrix/*README*
    share/openzone/lua/matrix/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/matrix COMPONENT data )

  file( GLOB files
    share/openzone/lua/nirvana/*.lua
    share/openzone/lua/nirvana/*README*
    share/openzone/lua/nirvana/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/nirvana COMPONENT data )

  # mdl
  file( GLOB files
    share/openzone/mdl/*.ozcSMM
    share/openzone/mdl/*.ozcMD2
    share/openzone/mdl/*README*
    share/openzone/mdl/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/mdl COMPONENT data )

  # music
  file( GLOB files
    share/openzone/music/*.oga
    share/openzone/music/*README*
    share/openzone/music/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/music COMPONENT data )

  # name
  file( GLOB files
    share/openzone/name/*.txt
    share/openzone/name/*README*
    share/openzone/name/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/name COMPONENT data )

  # snd
  file( GLOB files
    share/openzone/snd/*.wav
    share/openzone/snd/*README*
    share/openzone/snd/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/snd COMPONENT data )

  # terra
  file( GLOB files
    share/openzone/terra/*.ozTerra
    share/openzone/terra/*.ozcTerra
    share/openzone/terra/*README*
    share/openzone/terra/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/terra COMPONENT data )

  # ui
  file( GLOB files
    share/openzone/ui/cur/*.ozcCur
    share/openzone/ui/cur/*README*
    share/openzone/ui/cur/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/cur COMPONENT data )

  file( GLOB files
    share/openzone/ui/font/*.ttf
    share/openzone/ui/font/*README*
    share/openzone/ui/font/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/font COMPONENT data )

  file( GLOB files
    share/openzone/ui/icon/*.ozcTex
    share/openzone/ui/icon/*README*
    share/openzone/ui/icon/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/icon COMPONENT data )

  # locale
  file( GLOB dirs share/locale/* )
  foreach( dir IN ITEMS ${dirs} )
    file( RELATIVE_PATH dir ${CMAKE_SOURCE_DIR} ${dir} )
    install( FILES ${dir}/LC_MESSAGES/openzone.mo DESTINATION ${dir}/LC_MESSAGES COMPONENT data )
  endforeach()

endif()

#
# development data
#
if( OZ_INSTALL_DATA_SRC )

  install( FILES share/openzone/README DESTINATION share/openzone COMPONENT data-src )

  # caelum
  file( GLOB files
    share/openzone/caelum/*.png
    share/openzone/caelum/*.jpg
    share/openzone/caelum/*.xcf
    share/openzone/caelum/*README*
    share/openzone/caelum/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/caelum COMPONENT data-src )

  # class
  file( GLOB files
    share/openzone/class/*.rc
    share/openzone/class/*README*
    share/openzone/class/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/class COMPONENT data )

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
  install( FILES ${files} DESTINATION share/openzone/data/maps COMPONENT data_src )

  install( DIRECTORY share/openzone/data/scripts DESTINATION share/openzone/data/scripts COMPONENT data_src )

  file( GLOB files
    share/openzone/data/textures/oz/*.png
    share/openzone/data/textures/oz/*.jpg
    share/openzone/data/textures/oz/*README*
    share/openzone/data/textures/oz/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/data/textures/oz COMPONENT data_src )

  # lua
  file( GLOB files
    share/openzone/lua/matrix/*.lua
    share/openzone/lua/matrix/*README*
    share/openzone/lua/matrix/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/matrix COMPONENT data )

  file( GLOB files
    share/openzone/lua/nirvana/*.lua
    share/openzone/lua/nirvana/*README*
    share/openzone/lua/nirvana/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/lua/nirvana COMPONENT data )

  # music
  file( GLOB files
    share/openzone/music/*.oga
    share/openzone/music/*README*
    share/openzone/music/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/music COMPONENT data )

  # name
  file( GLOB files
    share/openzone/name/*.txt
    share/openzone/name/*README*
    share/openzone/name/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/name COMPONENT data )

  # snd
  file( GLOB files
    share/openzone/snd/*.wav
    share/openzone/snd/*README*
    share/openzone/snd/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/snd COMPONENT data )

  # terra
  file( GLOB files
    share/openzone/terra/*.rc
    share/openzone/terra/*.png
    share/openzone/terra/*.jpg
    share/openzone/terra/*README*
    share/openzone/terra/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/terra COMPONENT data_src )

  # netradiant
  install( DIRECTORY share/openzone/netradiant DESTINATION share/openzone COMPONENT data_src )

  # mdl
  file( GLOB files share/openzone/mdl/* )
  foreach( file IN ITEMS ${files} )
    if( NOT EXISTS ${file}/config.rc )
      list( REMOVE_ITEM files ${file} )
    endif()
  endforeach()
  install( DIRECTORY ${files} DESTINATION share/openzone/mdl COMPONENT data_src )

  # ui
  file( GLOB files
    share/openzone/ui/cur/*.in
    share/openzone/ui/cur/*.png
    share/openzone/ui/cur/*README*
    share/openzone/ui/cur/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/cur COMPONENT data_src )

  file( GLOB files
    share/openzone/ui/font/*.ttf
    share/openzone/ui/font/*README*
    share/openzone/ui/font/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/font COMPONENT data )

  file( GLOB files
    share/openzone/ui/icon/*.png
    share/openzone/ui/icon/*.xcf
    share/openzone/ui/icon/*README*
    share/openzone/ui/icon/*COPYING* )
  install( FILES ${files} DESTINATION share/openzone/ui/icon COMPONENT data_src )

  # locale
  file( GLOB dirs share/locale/* )
  foreach( dir IN ITEMS ${dirs} )
    file( RELATIVE_PATH dir ${CMAKE_SOURCE_DIR} ${dir} )
    install( FILES ${dir}/LC_MESSAGES/openzone.po ${dir}/LC_MESSAGES/openzone.mo
      DESTINATION ${dir}/LC_MESSAGES COMPONENT data_src )
  endforeach()

endif()
