link_libraries( base )

add_executable( test
  test.cpp )

add_dependencies( test precompile )
