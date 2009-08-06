link_libraries( base )

add_executable( test
  foreach.cpp )

add_dependencies( test precompile )
