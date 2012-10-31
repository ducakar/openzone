set( headers
  ozdynamics.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Mesh.hh
  collision/Object.hh
  collision/Shape.hh
  collision/Space.hh
  physics/Body.hh )
add_library( ozdynamics
  ozdynamics.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Mesh.hh
  collision/Object.hh
  collision/Shape.hh
  collision/Space.hh
  physics/Body.hh
  collision/Box.cc
  collision/Capsule.cc
  collision/Collider.cc
  collision/Compound.cc
  collision/Mesh.cc
  collision/Shape.cc )
set_target_properties( ozdynamics PROPERTIES
                       PUBLIC_HEADER "${headers}"
                       VERSION ${OZ_VERSION}
                       SOVERSION 0 )
target_link_libraries( ozdynamics oz )

install( TARGETS ozdynamics
         RUNTIME DESTINATION bin
         LIBRARY ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/ozdynamics )
