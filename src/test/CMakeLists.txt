link_libraries( base matrix nirvana )

add_executable( test
  test.cpp )

add_dependencies( test precompile )
