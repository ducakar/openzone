if( ANDROID )

  add_library( openzone SHARED openzone.cc )
  target_link_libraries( openzone client modules nirvana matrix common ozCore )
  use_pch( openzone pch )

  set( jniLibPath ${CMAKE_BINARY_DIR}/../Android/libs/${PLATFORM_PROC_PREFIX}/libopenzone.so )
  add_custom_command( OUTPUT ${jniLibPath} DEPENDS openzone
                      COMMAND ${CMAKE_COMMAND} -E copy libopenzone.so ${jniLibPath} )
  add_custom_target( openzone_android ALL DEPENDS android_project ${jniLibPath} )

elseif( NACL )

  add_executable( openzone openzone.cc )
  target_link_libraries( openzone client modules nirvana matrix common ozCore )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME openzone${PLATFORM_EXE_SUFFIX} )
  use_pch( openzone pch )

else()

  add_executable( openzone openzone.cc )
  target_link_libraries( openzone client modules nirvana matrix common ozDynamics )
  use_pch( openzone pch )

  install( TARGETS openzone RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozBuild ozBuild.cc )
  target_link_libraries( ozBuild builder client modules nirvana matrix common ozCore )
  use_pch( ozBuild pch )

  install( TARGETS ozBuild RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozGettext ozGettext.cc )
  target_link_libraries( ozGettext ozCore )

  install( TARGETS ozGettext RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  add_executable( ozManifest ozManifest.cc )
  target_link_libraries( ozManifest ozCore )

  install( TARGETS ozManifest RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR} )

  install( DIRECTORY ${CMAKE_SOURCE_DIR}/share/applications ${CMAKE_SOURCE_DIR}/share/pixmaps
           DESTINATION share )

endif()
