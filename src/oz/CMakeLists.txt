add_library( oz STATIC
  Alloc.cpp
  Buffer.cpp
  Config.cpp
  Exception.cpp
  File.cpp
  Log.cpp
  Mat44.cpp
  Math.cpp
  Point3.cpp
  Quat.cpp
  String.cpp
  System.cpp
  Vec3.cpp
  Vec4.cpp
  common.cpp
)

if( OZ_INSTALL_LIBOZ )
  install( FILES ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp DESTINATION include/oz )
  install( DIRECTORY . FILES_MATCHING PATTERN "*.hpp" DESTINATION include/oz )
  install( TARGETS oz DESTINATION lib )
endif()
