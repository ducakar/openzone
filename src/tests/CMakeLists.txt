add_executable( foreach foreach.cpp )
add_dependencies( foreach precompiled )
target_link_libraries( foreach base )

add_executable( io io.cpp )
add_dependencies( io precompiled )
target_link_libraries( io base )

add_executable( lua lua.cpp )
add_dependencies( lua precompiled )
target_link_libraries( lua base )

add_executable( pool pool.cpp )
add_dependencies( pool precompiled )
target_link_libraries( pool base )

add_executable( quicksort quicksort.cpp )
add_dependencies( quicksort precompiled )
target_link_libraries( quicksort base )

add_executable( register register.cpp )
add_dependencies( register precompiled )
target_link_libraries( register base )

add_executable( sparse sparse.cpp )
add_dependencies( sparse precompiled )
target_link_libraries( sparse base )

add_executable( test test.cpp )
add_dependencies( test precompiled )
target_link_libraries( test base )
