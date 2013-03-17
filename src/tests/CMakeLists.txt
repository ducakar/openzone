if( NOT OZ_TESTS )
  return()
endif()

add_executable( foreach foreach.cc )
target_link_libraries( foreach ozCore )

# include_directories( SYSTEM /usr/include/mono-2.0 )
# add_executable( mono mono.cc )
# target_link_libraries( mono -lmono-2.0 ozCore )

add_executable( quicksort quicksort.cc )
target_link_libraries( quicksort ozCore )

add_executable( simd simd.cc )
target_link_libraries( simd ozCore )

add_executable( test test.cc )
target_link_libraries( test ozEngine openal vorbisfile )
