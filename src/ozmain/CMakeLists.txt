add_library( ozmain STATIC main.cc )

if( NACL )
  target_link_libraries( ozmain ${PEPPER_GLES2_LIBRARY} ${PEPPER_CXX_LIBRARY} ${PEPPER_LIBRARY} )
endif()
