add_executable( foreach foreach.cc ../ozmain/main.cc )
target_link_libraries( foreach oz ozmain )

if( NACL )
  include_directories( /home/davorin/Projects/nacl_sdk/pepper_19/toolchain/linux_x86_newlib/x86_64-nacl/include )

  add_custom_command( OUTPUT nacl.nmf nacl.html
    DEPENDS "${CMAKE_SOURCE_DIR}/etc/nacl/nacl.nmf" "${CMAKE_SOURCE_DIR}/etc/nacl/nacl.html"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/etc/nacl/nacl.nmf" nacl.nmf
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/etc/nacl/nacl.html" nacl.html )
  add_custom_target( nacl_files DEPENDS nacl.nmf nacl.html )

  add_executable( nacl nacl.cc ../ozmain/main.cc )
  add_dependencies( nacl nacl_files )
  target_link_libraries( nacl oz ozmain )
  set_target_properties( nacl PROPERTIES OUTPUT_NAME "nacl.${CMAKE_SYSTEM_PROCESSOR}.nexe" )
endif()

add_executable( quicksort quicksort.cc ../ozmain/main.cc )
target_link_libraries( quicksort oz ozmain )

add_executable( simd simd.cc ../ozmain/main.cc )
target_link_libraries( simd oz ozmain )

add_executable( test test.cc ../ozmain/main.cc )
target_link_libraries( test oz ozmain )

add_executable( unittest unittest.cc ../ozmain/main.cc )
target_link_libraries( unittest oz ozmain )
