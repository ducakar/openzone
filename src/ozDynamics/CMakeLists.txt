configure_file( config.hh.in config.hh @ONLY )
configure_file( ozDynamics.pc.in ozDynamics.pc @ONLY )

add_library( ozDynamics
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  ozDynamics.hh
  collision/AABB.hh
  collision/Body.hh
  collision/Bounds.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Mesh.hh
  collision/Shape.hh
  collision/Space.hh
  physics/DBody.hh
  physics/Physics.hh
  collision/Body.cc
  collision/Box.cc
  collision/Capsule.cc
  collision/Collider.cc
  collision/Compound.cc
  collision/Mesh.cc
  collision/Shape.cc
  collision/Space.cc
  physics/DBody.cc
  physics/Physics.cc
#END SOURCES
)

set_target_properties( ozDynamics PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

# HACK.
if( OZ_DYNAMICS )
  target_link_libraries( ozDynamics ${ODE_LIBRARIES} ozCore )
else()
  target_link_libraries( ozDynamics ozCore )
endif()

install( TARGETS ozDynamics
         RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR} )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozDynamics )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozDynamics.pc DESTINATION lib/pkgconfig )
