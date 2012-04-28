add_executable( foreach foreach.cc )
target_link_libraries( foreach oz )

add_executable( quicksort quicksort.cc )
target_link_libraries( quicksort oz )

add_executable( simd simd.cc )
target_link_libraries( simd oz )

add_executable( test test.cc )
target_link_libraries( test oz )

add_executable( unittest unittest.cc )
target_link_libraries( unittest oz )
