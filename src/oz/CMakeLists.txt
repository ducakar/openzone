add_library( oz STATIC
  Alloc.cpp
  Buffer.cpp
  Config.cpp
  Directory.cpp
  Exception.cpp
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
  oz.cpp
)

if( OZ_INSTALL_LIBOZ )
  install( FILES
    ${CMAKE_BINARY_DIR}/src/oz/ozconfig.hpp
    Alloc.cpp
    Alloc.hpp
    Array.hpp
    Bitset.hpp
    Buffer.cpp
    Buffer.hpp
    Config.cpp
    Config.hpp
    DArray.hpp
    DList.hpp
    Directory.cpp
    Directory.hpp
    Exception.cpp
    Exception.hpp
    HashIndex.hpp
    HashString.hpp
    List.hpp
    Log.cpp
    Log.hpp
    Map.hpp
    Mat44.cpp
    Mat44.hpp
    Math.cpp
    Math.hpp
    Pair.hpp
    Plane.hpp
    Point3.cpp
    Point3.hpp
    Pool.hpp
    Quat.cpp
    Quat.hpp
    SBitset.hpp
    SVector.hpp
    Sparse.hpp
    String.cpp
    String.hpp
    System.cpp
    System.hpp
    Vec3.cpp
    Vec3.hpp
    Vec4.cpp
    Vec4.hpp
    Vector.hpp
    arrays.hpp
    common.cpp
    common.hpp
    iterables.hpp
    oz.cpp
    oz.hpp
    stream.hpp
    DESTINATION include/oz
    COMPONENT liboz )
  install( TARGETS oz DESTINATION lib COMPONENT liboz )
endif()
