if( NACL )

  add_custom_command( OUTPUT nacl.nmf nacl.html
    DEPENDS ${CMAKE_SOURCE_DIR}/etc/nacl/nacl.nmf ${CMAKE_SOURCE_DIR}/etc/nacl/nacl.html
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/etc/nacl/nacl.nmf nacl.nmf
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/etc/nacl/nacl.html nacl.html )
  add_custom_target( nacl_files DEPENDS nacl.nmf nacl.html )

  add_executable( nacl nacl.cc main.hh main.cc )
  add_dependencies( nacl nacl_files )
  target_link_libraries( nacl oz ${PEPPER_GLES2_LIBRARY} )
  set_target_properties( nacl PROPERTIES OUTPUT_NAME nacl.${CMAKE_SYSTEM_PROCESSOR}.nexe )

else()

  add_executable( foreach foreach.cc )
  target_link_libraries( foreach oz )

  add_executable( quicksort quicksort.cc )
  target_link_libraries( quicksort oz )

  add_executable( simd simd.cc )
  target_link_libraries( simd oz )

  add_executable( test test.cc )
  target_link_libraries( test oz )
  use_pch( test pch )

endif()
