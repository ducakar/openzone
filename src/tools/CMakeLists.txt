add_executable( openzone openzone.cpp )
add_dependencies( openzone client ui nirvana matrix build pch )
target_link_libraries( openzone client arch nirvana matrix build oz ${libs} )

if( OZ_INSTALL_BIN )
  install( TARGETS openzone DESTINATION bin )
endif()

if( OZ_BUILD_TOOLS )
  add_executable( ozPrebuild ozPrebuild.cpp )
  add_dependencies( ozPrebuild client ui nirvana matrix pch )
  target_link_libraries( ozPrebuild client nirvana matrix oz ${libs} )

  if( OZ_INSTALL_BIN )
    install( TARGETS ozPrebuild DESTINATION bin )
  endif()
endif()
