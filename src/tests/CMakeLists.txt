if( NACL )
  return()
endif()

add_executable( foreach foreach.cc )
target_link_libraries( foreach ozCore )

add_executable( quicksort quicksort.cc )
target_link_libraries( quicksort ozCore )

add_executable( simd simd.cc )
target_link_libraries( simd ozCore )

add_executable( test test.cc )
target_link_libraries( test ozCore )
