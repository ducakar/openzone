add_executable( ozSineSample ozSineSample.cc )
target_link_libraries( ozSineSample oz )

add_executable( ozFormat ClassFormatter.cc ozFormat.cc )
add_dependencies( ozFormat pch )
target_link_libraries( ozFormat matrix common oz ${libs_client} )

install( TARGETS ozFormat COMPONENT tools RUNTIME DESTINATION bin )
