link_libraries( base matrix nirvana )

add_executable( test
  script.cpp )

add_dependencies( test precompile )
