configure_file( config.hh.in config.hh @ONLY )
configure_file( ozDynamics.pc.in ozDynamics.pc @ONLY )

add_library( ozDynamics
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  common.hh
  ozDynamics.hh
  pch.hh
  common.cc
  collision/AABB.hh
  collision/Body.hh
  collision/Bounds.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Polytope.hh
  collision/Shape.hh
  collision/Space.hh
  physics/DBody.hh
  physics/Physics.hh
  collision/Body.cc
  collision/Box.cc
  collision/Capsule.cc
  collision/Collider.cc
  collision/Compound.cc
  collision/Polytope.cc
  collision/Shape.cc
  collision/Space.cc
  physics/DBody.cc
  physics/Physics.cc
#END SOURCES
)

target_link_libraries( ozDynamics ozCore )

# HACK
if( OZ_DYNAMICS )
  target_link_libraries( ozDynamics ${ODE_LIBRARIES} )
endif()

set_target_properties( ozDynamics PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

add_pch( ozDynamics_pch pch.hh pch.cc )
use_pch( ozDynamics ozDynamics_pch )

install( TARGETS ozDynamics
         RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR} )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozDynamics )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozDynamics.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig )
