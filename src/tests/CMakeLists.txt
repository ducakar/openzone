add_executable( test test.cpp )
add_dependencies( test precompiled )
target_link_libraries( test base matrix nirvana )
