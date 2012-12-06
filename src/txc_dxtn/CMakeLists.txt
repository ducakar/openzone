if( UNIX )
  add_library( txc_dxtn SHARED txc_dxtn.hh txc_dxtn.cc )

  if( OZ_STANDALONE )
    install( TARGETS txc_dxtn LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR} )
  endif()
endif()
