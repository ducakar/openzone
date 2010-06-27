add_executable( alloctest alloctest.cc )
add_dependencies( alloctest pch )
target_link_libraries( alloctest matrix oz )

add_executable( foreach foreach.cc )
add_dependencies( foreach pch )
target_link_libraries( foreach oz )

add_executable( io io.cc )
add_dependencies( io pch )
target_link_libraries( io matrix oz )

add_executable( lua lua.cc )
add_dependencies( lua pch )
target_link_libraries( lua oz )

add_executable( pool pool.cc )
add_dependencies( pool pch )
target_link_libraries( pool oz )

add_executable( quicksort quicksort.cc )
add_dependencies( quicksort pch )
target_link_libraries( quicksort oz )

add_executable( register register.cc )
add_dependencies( register pch )
target_link_libraries( register oz )

add_executable( sparse sparse.cc )
add_dependencies( sparse pch )
target_link_libraries( sparse oz )

add_executable( test test.cc )
add_dependencies( test pch )
target_link_libraries( test oz )
