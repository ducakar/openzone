add_executable( alloctest alloctest.cc )
add_dependencies( alloctest pch )
target_link_libraries( alloctest matrix common oz ${libs_client} )

add_executable( foreach foreach.cc )
add_dependencies( foreach pch )
target_link_libraries( foreach oz ${libs} )

add_executable( pool pool.cc )
add_dependencies( pool pch )
target_link_libraries( pool oz ${libs} )

add_executable( quicksort quicksort.cc )
add_dependencies( quicksort pch )
target_link_libraries( quicksort oz ${libs_client} )

add_executable( simd simd.cc )
add_dependencies( simd pch )
target_link_libraries( simd oz ${libs} )

add_executable( unittest unittest.cc )
add_dependencies( unittest pch )
target_link_libraries( unittest oz ${libs} )

add_executable( test test.cc )
add_dependencies( test pch )

target_link_libraries( test oz ${libs} )
