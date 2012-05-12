add_library( ozmain STATIC main.cc )

if( NACL )
  target_link_libraries( ozmain ${PEPPER_CXX_LIBRARY} ${PEPPER_LIBRARY} )
endif()
