add_library( oz STATIC
  Alloc.cc
  Buffer.cc
  Config.cc
  Exception.cc
  File.cc
  Log.cc
  Mat44.cc
  Math.cc
  PhysFile.cc
  Point3.cc
  Quat.cc
  StackTrace.cc
  String.cc
  System.cc
  Vec3.cc
  Vec4.cc
  common.cc
)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz COMPONENT liboz )
install( DIRECTORY . DESTINATION include/oz COMPONENT liboz FILES_MATCHING PATTERN "*.hh" )
install( TARGETS oz DESTINATION lib COMPONENT liboz )
install( FILES ${PROJECT_SOURCE_DIR}/etc/liboz/liboz.pc DESTINATION lib/pkgconfig COMPONENT liboz )
install( FILES ${PROJECT_SOURCE_DIR}/etc/liboz/COPYING DESTINATION doc/liboz COMPONENT liboz )
