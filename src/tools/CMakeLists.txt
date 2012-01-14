add_executable( ozSineSample ozSineSample.cc )
target_link_libraries( ozSineSample oz ${libs} )

add_executable( ozFormat ClassFormatter.cc ozFormat.cc )
add_dependencies( ozFormat pch )
target_link_libraries( ozFormat matrix common oz ${libs_client} )

if( OZINST_TOOLS )
  install( TARGETS ozFormat DESTINATION bin )
endif()
