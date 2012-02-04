configure_file( ozconfig.hh.in ozconfig.hh )

add_library( oz SHARED
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
  Time.cc
  Vec3.cc
  Vec4.cc
  common.cc
)
target_link_libraries( oz ${libs_oz} )
set_target_properties( oz PROPERTIES SOVERSION ${OZ_SOVERSION} )

if( OZ_INSTALL_LIBOZ )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz )
  install( DIRECTORY . DESTINATION include/oz FILES_MATCHING PATTERN "*.hh" )
  install( TARGETS oz DESTINATION lib${OZ_LIB_SUFFIX} )
endif()
