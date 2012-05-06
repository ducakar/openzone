add_library( ozmain STATIC main.cc )

if( NACL )
  target_link_libraries( ozmain ${SDL_LIBRARY} ${PEPPER_CXX_LIBRARY} ${PEPPER_LIBRARY} )
endif()
