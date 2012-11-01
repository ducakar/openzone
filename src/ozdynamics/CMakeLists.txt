add_library( ozdynamics
  ozdynamics.hh
  collision/Body.hh
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
  collision/Space.cc )
set_target_properties( ozdynamics PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )
target_link_libraries( ozdynamics oz )

install( TARGETS ozdynamics
         RUNTIME DESTINATION bin COMPONENT libozdynmics
         LIBRARY DESTINATION lib COMPONENT libozdynmics
         ARCHIVE DESTINATION lib COMPONENT libozdynmics )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include COMPONENT libozdynamics
         FILES_MATCHING PATTERN *.hh )
install( FILES COPYING DESTINATION share/doc/libozdynamics-${OZ_VERSION} COMPONENT libozdynamics )
