configure_file( config.hh.in config.hh @ONLY )
configure_file( ozCore.pc.in ozCore.pc @ONLY )

add_library( ozCore
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  Alloc.hh
  Array.hh
  arrays.hh
  Bitset.hh
  Buffer.hh
  Chain.hh
  clearmacros.hh
  common.hh
  DArray.hh
  DChain.hh
  Endian.hh
  File.hh
  Gettext.hh
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
  Plane.hh
  Point.hh
  Pool.hh
  Quat.hh
  SBitset.hh
  Semaphore.hh
  Set.hh
  SharedLib.hh
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
  Gettext.cc
  JSON.cc
  Log.cc
  Mat33.cc
  Mat44.cc
  Math.cc
  Mutex.cc
  Point.cc
  Quat.cc
  Semaphore.cc
  SharedLib.cc
  StackTrace.cc
  String.cc
  System.cc
  Thread.cc
  Time.cc
  Vec3.cc
  Vec4.cc
#END SOURCES
)

if( ANDROID )
  target_link_libraries( ozCore ${PLATFORM_STL_LIBRARY} -llog -lOpenSLES -ldl )
elseif( NACL )
  target_link_libraries( ozCore -lpthread -lppapi -lppapi_cpp )
elseif( WIN32 )
  target_link_libraries( ozCore -lwinmm )
elseif( NOT CMAKE_SYSTEM_NAME STREQUAL Linux )
  target_link_libraries( ozCore -lpthread -lrt -lpulse-simple -ldl )
else()
  # TODO: Remove -lrt once glibc 2.17 becomes minimum.
  target_link_libraries( ozCore -lpthread -lrt -lasound -lpulse-simple -ldl )
endif()

target_link_libraries( ozCore ${PHYSFS_LIBRARY} )
if( PLATFORM_EMBEDDED )
  target_link_libraries( ozCore ${ZLIB_LIBRARY} )
endif()

set_target_properties( ozCore PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

install( TARGETS ozCore
         RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR} )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozCore )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozCore.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig )
