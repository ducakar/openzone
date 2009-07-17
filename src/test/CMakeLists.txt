link_libraries( base )

add_executable( test
  io.cpp )

add_dependencies( test precompile )
