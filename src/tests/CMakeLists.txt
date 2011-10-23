if( OZ_BUILD_TESTS )
#   add_executable( alloctest alloctest.cpp )
#   add_dependencies( alloctest pch )
#   target_link_libraries( alloctest matrix oz ${libs} )

  add_executable( foreach foreach.cpp )
  add_dependencies( foreach pch )
  target_link_libraries( foreach oz ${libs} )

  add_executable( pool pool.cpp )
  add_dependencies( pool pch )
  target_link_libraries( pool oz ${libs} )

  add_executable( quicksort quicksort.cpp )
  add_dependencies( quicksort pch )
  target_link_libraries( quicksort oz ${libs} )

  add_executable( simd simd.cpp )
  add_dependencies( simd pch )
  target_link_libraries( simd oz ${libs} )

  add_executable( unittest unittest.cpp )
  add_dependencies( unittest pch )
  target_link_libraries( unittest oz ${libs} )
endif()

if( OZ_BUILD_TESTS )
  add_executable( test test.cpp )
  add_dependencies( test pch )

  target_link_libraries( test build oz ${libs} )
endif()
