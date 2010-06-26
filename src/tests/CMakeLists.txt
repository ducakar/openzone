add_executable( alloctest alloctest.cpp )
add_dependencies( alloctest pch )
target_link_libraries( alloctest matrix oz )

add_executable( foreach foreach.cpp )
add_dependencies( foreach pch )
target_link_libraries( foreach oz )

add_executable( io io.cpp )
add_dependencies( io pch )
target_link_libraries( io matrix oz )

add_executable( lua lua.cpp )
add_dependencies( lua pch )
target_link_libraries( lua oz )

add_executable( pool pool.cpp )
add_dependencies( pool pch )
target_link_libraries( pool oz )

add_executable( quicksort quicksort.cpp )
add_dependencies( quicksort pch )
target_link_libraries( quicksort oz )

add_executable( register register.cpp )
add_dependencies( register pch )
target_link_libraries( register oz )

add_executable( sparse sparse.cpp )
add_dependencies( sparse pch )
target_link_libraries( sparse oz )

add_executable( test test.cpp )
add_dependencies( test pch )
target_link_libraries( test oz )
