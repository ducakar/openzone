#
# info files
#
if( WIN32 OR OZINST_STANDALONE )
  file( GLOB files README.* )
  install( FILES
    COPYING
    README
    ${files}
    DESTINATION . )
elseif( OZINST_INFO )
  file( GLOB files README.* )
  install( FILES
    AUTHORS
    COPYING
    README
    ${files}
    DESTINATION share/doc/openzone )
endif()

#
# icons
#
if( OZINST_MENU AND NOT WIN32 )
  install( DIRECTORY share/applications share/pixmaps DESTINATION share )
endif()

#
# oalinst, DLLs, localisation
#
if( OZINST_STANDALONE )
  if( OZINST_CLIENT )
    if( WIN32 )
      install( DIRECTORY support/mingw32-client/ DESTINATION bin )
      install( FILES support/oalinst.exe support/openzone.bat DESTINATION . )
    else()
      install( FILES support/openzone.sh PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION . )
    endif()
  endif()

  if( OZINST_TOOLS )
    if( WIN32 )
      install( DIRECTORY support/mingw32-tools/ DESTINATION bin )
      install( FILES support/build.bat DESTINATION . )
    else()
      install( FILES support/build.sh PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION . )
    endif()
  endif()
endif()
