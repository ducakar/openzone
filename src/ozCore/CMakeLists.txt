configure_file( config.hh.in config.hh )

add_library( ozCore
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  Alloc.hh
  Array.hh
  arrays.hh
  Bitset.hh
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
  ozCore.hh
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
  Vec4.cc
#END SOURCES
)

set_target_properties( ozCore PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

if( ANDROID )
  target_link_libraries( ozCore ${PLATFORM_STL_LIBRARY}
                                ${ANDROID_LOG_LIBRARY}
                                ${OPENSLES_LIBRARY}
                                ${PHYSFS_LIBRARY}
                                ${ZLIB_LIBRARY} )
elseif( NACL )
  target_link_libraries( ozCore ${PTHREAD_LIBRARY}
                                ${PHYSFS_LIBRARY}
                                ${ZLIB_LIBRARY}
                                ${PEPPER_CXX_LIBRARY}
                                ${PEPPER_LIBRARY} )
elseif( WIN32 )
  target_link_libraries( ozCore ${WINMM_LIBRARY}
                                ${PHYSFS_LIBRARY} )
else()
  target_link_libraries( ozCore ${PTHREAD_LIBRARY}
                                ${RT_LIBRARY}
                                ${DL_LIBRARY}
                                ${ALSA_LIBRARY}
                                ${PHYSFS_LIBRARY} )
endif()

install( TARGETS ozCore
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozCore )
install( FILES COPYING DESTINATION share/doc/liboz-${OZ_VERSION} )
