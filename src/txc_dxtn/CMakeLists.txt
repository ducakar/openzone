if( NOT OZ_STANDALONE OR NOT UNIX OR APPLE )
  return()
endif()

add_library( txc_dxtn SHARED txc_dxtn.hh txc_dxtn.cc )

install( TARGETS txc_dxtn LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR} )
