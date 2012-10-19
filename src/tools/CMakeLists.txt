if( OZ_STANDALONE )
  set( suffix "/${OZ_PLATFORM_NAME}" )
else()
  set( suffix "" )
endif()

if( ANDROID )
  add_library( openzone SHARED openzone.cc )

  set( jniLibPath ${CMAKE_BINARY_DIR}/../Android/libs/${PLATFORM_PROC_PREFIX}/libopenzone.so )
  add_custom_command( OUTPUT ${jniLibPath} DEPENDS openzone
                      COMMAND ${CMAKE_COMMAND} -E copy libopenzone.so ${jniLibPath} )
  add_custom_target( openzone_android ALL DEPENDS android_project ${jniLibPath} )
elseif( NACL )
  add_executable( openzone openzone.cc )
  set_target_properties( openzone PROPERTIES OUTPUT_NAME openzone.${CMAKE_SYSTEM_PROCESSOR}.nexe )
else()
  add_executable( openzone openzone.cc )
endif()
use_pch( openzone pch )
target_link_libraries( openzone client modules nirvana matrix common build_info oz ${libs_client} )

install( TARGETS openzone RUNTIME DESTINATION bin${suffix} LIBRARY DESTINATION lib${suffix} )

if( NOT ANDROID AND NOT NACL )

  add_executable( ozGettext ozGettext.cc )
  target_link_libraries( ozGettext oz )

  install( TARGETS ozGettext RUNTIME DESTINATION bin${suffix} )

  add_executable( ozManifest ozManifest.cc )
  target_link_libraries( ozManifest oz )

  install( TARGETS ozManifest RUNTIME DESTINATION bin${suffix} )

endif()
