configure_file( liboz.pc.in liboz.pc )
configure_file( ozconfig.hh.in ozconfig.hh )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set( headers
  ${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh
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
  File.hh
  HashMap.hh
  HashSet.hh
  InputStream.hh
  iterables.hh
  JSON.hh
  List.hh
  Log.hh
  Map.hh
  Mat33.hh
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
  Set.hh
  simd.hh
  SList.hh
  SpinLock.hh
  StackTrace.hh
  String.hh
  System.hh
  Thread.hh
  Time.hh
  Vec3.hh
  Vec4.hh )
add_library( oz
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
  File.hh
  HashMap.hh
  HashSet.hh
  InputStream.hh
  iterables.hh
  JSON.hh
  List.hh
  Log.hh
  Map.hh
  Mat33.hh
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
  Set.hh
  simd.hh
  SList.hh
  SpinLock.hh
  StackTrace.hh
  String.hh
  System.hh
  Thread.hh
  Time.hh
  Vec3.hh
  Vec4.hh
  Alloc.cc
  Buffer.cc
  common.cc
  File.cc
  JSON.cc
  Log.cc
  Mat33.cc
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
set_target_properties( oz PROPERTIES
                       RESOURCE ${CMAKE_CURRENT_BINARY_DIR}/liboz.pc
                       VERSION ${OZ_VERSION}
                       SOVERSION 0 )
target_link_libraries( oz ${libs_oz} )

install( TARGETS oz
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/oz
         RESOURCE DESTINATION lib/pkgconfig )
