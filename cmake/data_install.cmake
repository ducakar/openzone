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
  install( DIRECTORY share/openzone/glsl DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )
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
    install( DIRECTORY support/mingw32-client/ DESTINATION bin )

    if( OZ_INSTALL_STANDALONE )
      install( FILES support/oalinst.exe support/openzone.bat DESTINATION . )
    endif()
  endif()

  if( OZ_INSTALL_TOOLS )
    install( DIRECTORY support/mingw32-tools/ DESTINATION bin )

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
  install( DIRECTORY share/openzone/bsp DESTINATION share/openzone )

  # caelum
  install( DIRECTORY share/openzone/caelum DESTINATION share/openzone
           FILES_MATCHING PATTERN "caelum/*.ozcCaelum" )

  install( DIRECTORY share/openzone/caelum DESTINATION share/openzone
           FILES_MATCHING PATTERN "caelum/*README*" )

  install( DIRECTORY share/openzone/caelum DESTINATION share/openzone
           FILES_MATCHING PATTERN "caelum/*COPYING*" )

  # class
  install( DIRECTORY share/openzone/class DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # lua
  install( DIRECTORY share/openzone/lua DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # mdl
  install( DIRECTORY share/openzone/mdl DESTINATION share/openzone
           FILES_MATCHING PATTERN "mdl/*.ozcSMM" )

  install( DIRECTORY share/openzone/mdl DESTINATION share/openzone
           FILES_MATCHING PATTERN "mdl/*.ozcMD2" )

  install( DIRECTORY share/openzone/mdl DESTINATION share/openzone
           FILES_MATCHING PATTERN "mdl/*README*" )

  install( DIRECTORY share/openzone/mdl DESTINATION share/openzone
           FILES_MATCHING PATTERN "mdl/*COPYING*" )

  # music
  install( DIRECTORY share/openzone/music DESTINATION share/openzone
           PATTERN "ALL" EXCLUDE )

  # name
  install( DIRECTORY share/openzone/name DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # snd
  install( DIRECTORY share/openzone/snd DESTINATION share/openzone
           PATTERN "ALL" EXCLUDE )

  # terra
  install( DIRECTORY share/openzone/terra DESTINATION share/openzone
           FILES_MATCHING PATTERN "terra/*.ozTerra" )

  install( DIRECTORY share/openzone/terra DESTINATION share/openzone
           FILES_MATCHING PATTERN "terra/*.ozcTerra" )

  install( DIRECTORY share/openzone/terra DESTINATION share/openzone
           FILES_MATCHING PATTERN "terra/*README*" )

  install( DIRECTORY share/openzone/terra DESTINATION share/openzone
           FILES_MATCHING PATTERN "terra/*COPYING*" )

  # ui
  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/cur/*.ozcCur" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/cur/*README*" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/cur/*COPYING*" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/font/*.ttf" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/font/*README*" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/font/*COPYING*" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/icon/*.ozcTex" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/icon/*README*" )

  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           FILES_MATCHING PATTERN "ui/*/icon/*COPYING*" )

  # locale
  install( DIRECTORY share/locale DESTINATION share
           FILES_MATCHING PATTERN "*.mo" )

endif()

#
# development data
#
if( OZ_INSTALL_DATA_SRC )

  install( FILES share/openzone/README DESTINATION share/openzone )

  # caelum
  install( DIRECTORY share/openzone/caelum DESTINATION share/openzone
           PATTERN "*.ozcCaelum" EXCLUDE )

  # class
  install( DIRECTORY share/openzone/class DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # data
  install( DIRECTORY share/openzone/data/maps DESTINATION share/openzone/data
           PATTERN "DISABLED" EXCLUDE
           PATTERN "*.bsp" EXCLUDE
           PATTERN "*.prt" EXCLUDE
           PATTERN "*.srf" EXCLUDE
           PATTERN "*autosave.map" EXCLUDE
           PATTERN "*.bak" EXCLUDE )

  install( DIRECTORY share/openzone/data/scripts DESTINATION share/openzone/data
           PATTERN "DISABLED" EXCLUDE )

  install( DIRECTORY share/openzone/data/textures DESTINATION share/openzone/data
           PATTERN "DISABLED" EXCLUDE )

  # lua
  install( DIRECTORY share/openzone/lua DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # music
  install( DIRECTORY share/openzone/music DESTINATION share/openzone
           PATTERN "ALL" EXCLUDE )

  # name
  install( DIRECTORY share/openzone/name DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE )

  # snd
  install( DIRECTORY share/openzone/snd DESTINATION share/openzone
           PATTERN "ALL" EXCLUDE )

  # terra
  install( DIRECTORY share/openzone/terra DESTINATION share/openzone
           PATTERN "*.ozTerra" EXCLUDE
           PATTERN "*.ozcTerra" EXCLUDE )

  # netradiant
  install( DIRECTORY share/openzone/netradiant DESTINATION share/openzone )

  # mdl
  install( DIRECTORY share/openzone/mdl DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE
           PATTERN "mdl/*.ozcSMM" EXCLUDE
           PATTERN "mdl/*.ozcMD2" EXCLUDE )

  # ui
  install( DIRECTORY share/openzone/ui DESTINATION share/openzone
           PATTERN "DISABLED" EXCLUDE
           PATTERN "*.ozcCur" EXCLUDE
           PATTERN "*.ozcTex" EXCLUDE )

  # locale
  install( DIRECTORY share/locale DESTINATION share )

  # launcher, icon
  install( DIRECTORY share/applications share/icons DESTINATION share )

  # support files (DLLs, launcher scripts ...)
  install( DIRECTORY support DESTINATION . )

endif()
