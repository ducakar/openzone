configure_file( ozconfig.hh.in ozconfig.hh )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

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
set_target_properties( oz PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

if( ANDROID )
  target_link_libraries( oz ${PLATFORM_STL_LIBRARY}
                            ${ANDROID_LOG_LIBRARY}
                            ${OPENSLES_LIBRARY}
                            ${PHYSFS_LIBRARY}
                            ${ZLIB_LIBRARY} )
elseif( NACL )
  target_link_libraries( oz ${PTHREAD_LIBRARY}
                            ${PHYSFS_LIBRARY}
                            ${ZLIB_LIBRARY}
                            ${PEPPER_CXX_LIBRARY}
                            ${PEPPER_LIBRARY} )
elseif( WIN32 )
  target_link_libraries( oz ${WINMM_LIBRARY}
                            ${PHYSFS_LIBRARY} )
else()
  target_link_libraries( oz ${PTHREAD_LIBRARY}
                            ${RT_LIBRARY}
                            ${DL_LIBRARY}
                            ${ALSA_LIBRARY}
                            ${PHYSFS_LIBRARY} )
endif()

install( TARGETS oz
         RUNTIME DESTINATION bin COMPONENT liboz
         LIBRARY DESTINATION lib COMPONENT liboz
         ARCHIVE DESTINATION lib COMPONENT liboz )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include COMPONENT liboz
         FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz COMPONENT liboz )
install( FILES COPYING DESTINATION share/doc/liboz-${OZ_VERSION} COMPONENT liboz )
