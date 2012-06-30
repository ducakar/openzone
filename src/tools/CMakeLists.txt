add_executable( ozGettext ozGettext.cc )
target_link_libraries( ozGettext oz )

install( TARGETS ozGettext RUNTIME DESTINATION bin )

add_executable( ozManifest ozManifest.cc )
target_link_libraries( ozManifest oz )

install( TARGETS ozManifest RUNTIME DESTINATION bin )

add_executable( ozSineSample ozSineSample.cc )
target_link_libraries( ozSineSample oz )
