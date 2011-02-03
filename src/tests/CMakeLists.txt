add_executable( alloctest alloctest.cpp )
add_dependencies( alloctest matrix pch )
target_link_libraries( alloctest matrix oz ${libs} )

add_executable( foreach foreach.cpp )
add_dependencies( foreach pch )
target_link_libraries( foreach oz ${libs} )

add_executable( io io.cpp )
add_dependencies( io matrix pch )
target_link_libraries( io oz ${libs} )

add_executable( lua lua.cpp )
add_dependencies( lua pch )
target_link_libraries( lua oz ${libs} )

add_executable( pool pool.cpp )
add_dependencies( pool pch )
target_link_libraries( pool oz ${libs} )

add_executable( quicksort quicksort.cpp )
add_dependencies( quicksort pch )
target_link_libraries( quicksort oz ${libs} )

add_executable( register register.cpp )
add_dependencies( register pch )
target_link_libraries( register oz ${libs} )

add_executable( simd simd.cpp )
add_dependencies( simd pch )
target_link_libraries( simd oz ${libs} )

add_executable( sparse sparse.cpp )
add_dependencies( sparse pch )
target_link_libraries( sparse oz ${libs} )

add_executable( test test.cpp )
add_dependencies( test pch )
target_link_libraries( test oz ${libs} )

add_executable( unittest unittest.cpp )
add_dependencies( unittest pch )
target_link_libraries( unittest oz ${libs} )
