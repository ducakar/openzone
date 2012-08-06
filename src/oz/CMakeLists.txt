configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

set( ozHeaders
  AABB.hh
  Alloc.hh
  Array.hh
  arrays.hh
  Bitset.hh
  Bounds.hh
  Buffer.hh
  BufferStream.hh
  Chain.hh
  clearmacros.hh
  common.hh
  DArray.hh
  DChain.hh
  Endian.hh
  Exception.hh
  File.hh
  HashIndex.hh
  HashString.hh
  InputStream.hh
  iterables.hh
  JSON.hh
  List.hh
  Log.hh
  Map.hh
  Mat44.hh
  Math.hh
  Mutex.hh
  OutputStream.hh
  oz.hh
  Pair.hh
  PFile.hh
  Plane.hh
  Point.hh
  Pool.hh
  Quat.hh
  SBitset.hh
  Semaphore.hh
  Simd.hh
  SList.hh
  StackTrace.hh
  String.hh
  System.hh
  Thread.hh
  Time.hh
  Vec3.hh
  Vec4.hh )
add_library( oz
  Alloc.cc
  Buffer.cc
  common.cc
  Exception.cc
  File.cc
  JSON.cc
  Log.cc
  Mat44.cc
  Math.cc
  Mutex.cc
  PFile.cc
  Point.cc
  Quat.cc
  Semaphore.cc
  StackTrace.cc
  String.cc
  System.cc
  Thread.cc
  Time.cc
  Vec3.cc
  Vec4.cc )
set_target_properties( oz PROPERTIES PUBLIC_HEADER "${ozHeaders}" )
set_target_properties( oz PROPERTIES VERSION "${OZ_VERSION}" SOVERSION "0" )
target_link_libraries( oz ${libs_oz} )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/liboz.pc" DESTINATION lib/pkgconfig )
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh" DESTINATION include/oz )
install( TARGETS oz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/oz )
