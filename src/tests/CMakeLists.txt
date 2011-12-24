add_executable( foreach foreach.cc )
target_link_libraries( foreach oz ${libs} )

add_executable( quicksort quicksort.cc )
target_link_libraries( quicksort oz ${libs_client} )

add_executable( simd simd.cc )
target_link_libraries( simd oz ${libs} )

add_executable( unittest unittest.cc )
target_link_libraries( unittest oz ${libs} )

add_executable( test test.cc )
target_link_libraries( test oz ${libs} )
