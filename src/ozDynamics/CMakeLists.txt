add_library( ozDynamics
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
  collision/Body.cc
  collision/Box.cc
  collision/Capsule.cc
  collision/Collider.cc
  collision/Compound.cc
  collision/Mesh.cc
  collision/Shape.cc
  collision/Space.cc
  physics/DBody.cc
#END SOURCES
)

set_target_properties( ozDynamics PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )
target_link_libraries( ozDynamics ozCore )

install( TARGETS ozDynamics
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES COPYING DESTINATION share/doc/liboz-${OZ_VERSION} )
