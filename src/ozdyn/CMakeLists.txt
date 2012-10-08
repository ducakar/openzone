configure_file( libozdyn.pc.in libozdyn.pc )

set( headers
  ozdyn.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Shape.hh
  physics/Body.hh )
add_library( ozdyn
  ozdyn.hh
  collision/Box.hh
  collision/Capsule.hh
  collision/Collider.hh
  collision/Compound.hh
  collision/Shape.hh
  physics/Body.hh
  collision/Box.cc
  collision/Capsule.cc
  collision/Collider.cc
  collision/Compound.cc )

set_target_properties( ozdyn PROPERTIES
  PUBLIC_HEADER "${headers}"
  RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/libozdyn.pc"
  VERSION "${OZ_VERSION}"
  SOVERSION "0" )
target_link_libraries( ozdyn oz )

install( TARGETS ozdyn
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/ozdyn
         RESOURCE DESTINATION lib/pkgconfig )
