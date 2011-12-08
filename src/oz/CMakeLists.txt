add_library( oz STATIC
  Alloc.cc
  Buffer.cc
  Config.cc
  Exception.cc
  File.cc
  Log.cc
  Mat44.cc
  Math.cc
  Point3.cc
  Quat.cc
  StackTrace.cc
  String.cc
  System.cc
  Vec3.cc
  Vec4.cc
  common.cc
)

if( OZ_INSTALL_LIBOZ )
  install( FILES ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hh DESTINATION include/oz )
  install( DIRECTORY . DESTINATION include/oz FILES_MATCHING PATTERN "*.hh" )
  install( TARGETS oz DESTINATION lib )
endif()
