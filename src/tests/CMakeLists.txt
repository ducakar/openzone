add_executable( foreach foreach.cc )
target_link_libraries( foreach oz )

if( NACL )
  include_directories( /home/davorin/Projects/nacl_sdk/pepper_19/toolchain/linux_x86_newlib/x86_64-nacl/include )

  add_custom_command( OUTPUT nacl.nmf nacl.html
    DEPENDS "${CMAKE_SOURCE_DIR}/etc/nacltest/nacl.nmf" "${CMAKE_SOURCE_DIR}/etc/nacltest/nacl.html"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/etc/nacltest/nacl.nmf" nacl.nmf
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/etc/nacltest/nacl.html" nacl.html )
  add_custom_target( nacl_files DEPENDS nacl.nmf nacl.html )

  add_executable( "nacl.${CMAKE_SYSTEM_PROCESSOR}.nexe" nacl.cc ../ozmain/main.cc )
  add_dependencies( "nacl.${CMAKE_SYSTEM_PROCESSOR}.nexe" nacl_files )
  target_link_libraries( "nacl.${CMAKE_SYSTEM_PROCESSOR}.nexe" oz ${SDL_LIBRARY} ${PEPPER_CXX_LIBRARY} ${PEPPER_LIBRARY} )
endif()

add_executable( quicksort quicksort.cc )
target_link_libraries( quicksort oz )

add_executable( simd simd.cc )
target_link_libraries( simd oz )

add_executable( test test.cc )
target_link_libraries( test oz )

add_executable( unittest unittest.cc )
target_link_libraries( unittest oz )
