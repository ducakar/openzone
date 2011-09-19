if( NOT OZ_SDK )
  add_executable( openzone openzone.cpp )
  add_dependencies( openzone client ui nirvana matrix build pch )
  target_link_libraries( openzone client nirvana matrix build oz ${libs} )

  install( TARGETS openzone DESTINATION bin )
else()
  add_executable( ozPrebuild ozPrebuild.cpp )
  add_dependencies( ozPrebuild client ui nirvana matrix pch )
  target_link_libraries( ozPrebuild client nirvana matrix build oz ${libs} )

  install( TARGETS ozPrebuild DESTINATION bin )
endif()
