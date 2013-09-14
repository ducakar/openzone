if( EMSCRIPTEN )

  add_executable( openzone openzone.cc )
  use_pch( openzone pch )
  target_link_libraries( openzone client modules nirvana matrix common ozEngine )

elseif( ANDROID )

  add_library( openzone SHARED openzone.cc )
  use_pch( openzone pch )
  target_link_libraries( openzone client modules nirvana matrix common ozEngine )

  set( jniLibPath ${CMAKE_BINARY_DIR}/../Android/libs/${PLATFORM_PROC_PREFIX}/libopenzone.so )
  add_custom_command( OUTPUT ${jniLibPath} DEPENDS openzone
                      COMMAND ${CMAKE_COMMAND} -E copy libopenzone.so ${jniLibPath} )
  add_custom_target( openzone_android ALL DEPENDS android_project ${jniLibPath} )

elseif( NACL )

  add_executable( openzone openzone.cc )
  use_pch( openzone pch )
  target_link_libraries( openzone client modules nirvana matrix common ozEngine )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME openzone${PLATFORM_EXE_SUFFIX} )

else()

  add_executable( openzone openzone.cc )
  use_pch( openzone pch )
  target_link_libraries( openzone client modules nirvana matrix common ozEngine )
  install( TARGETS openzone RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozBuild ozBuild.cc )
  use_pch( openzone pch )
  target_link_libraries( ozBuild builder client modules nirvana matrix common ozFactory ozEngine )
  install( TARGETS ozBuild RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozDDS ozDDS.cc )
  target_link_libraries( ozDDS ozFactory )
  install( TARGETS ozDDS RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozGenEnvMap ozGenEnvMap.cc )
  target_link_libraries( ozGenEnvMap ozFactory )
  install( TARGETS ozGenEnvMap RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozGettext ozGettext.cc )
  target_link_libraries( ozGettext ozCore )
  install( TARGETS ozGettext RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozManifest ozManifest.cc )
  target_link_libraries( ozManifest ozCore )
  install( TARGETS ozManifest RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  install( DIRECTORY ${CMAKE_SOURCE_DIR}/share/applications ${CMAKE_SOURCE_DIR}/share/pixmaps
           DESTINATION share )

endif()

if( OZ_DYNAMICS )
  target_link_libraries( openzone ozDynamics )
endif()
