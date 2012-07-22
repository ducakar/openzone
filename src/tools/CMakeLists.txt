add_executable( ozGettext ozGettext.cc )
target_link_libraries( ozGettext oz )

if( OZ_STANDALONE )
  install( TARGETS ozGettext RUNTIME DESTINATION bin/${OZ_SYSTEM_NAME} )
else()
  install( TARGETS ozGettext RUNTIME DESTINATION bin )
endif()

add_executable( ozManifest ozManifest.cc )
target_link_libraries( ozManifest oz )

if( OZ_STANDALONE )
  install( TARGETS ozManifest RUNTIME DESTINATION bin/${OZ_SYSTEM_NAME} )
else()
  install( TARGETS ozManifest RUNTIME DESTINATION bin )
endif()

add_executable( ozSineSample ozSineSample.cc )
target_link_libraries( ozSineSample oz )
