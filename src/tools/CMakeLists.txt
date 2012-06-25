add_executable( ozGettext ozGettext.cc )
target_link_libraries( ozGettext oz )

install( TARGETS ozGettext RUNTIME DESTINATION bin )

add_executable( ozSineSample ozSineSample.cc )
target_link_libraries( ozSineSample oz )
