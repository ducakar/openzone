configure_file(config.hh.in config.hh @ONLY)
configure_file(ozCore.pc.in ozCore.pc @ONLY)

add_library(ozCore
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  Arrays.hh
  Atomic.hh
  Barrier.cc
  Barrier.hh
  Bitset.cc
  Bitset.hh
  CallOnce.hh
  Chain.hh
  DChain.hh
  Duration.cc
  Duration.hh
  Endian.hh
  EnumMap.cc
  EnumMap.hh
  File.cc
  File.hh
  Gettext.cc
  Gettext.hh
  HashMap.hh
  HashSet.hh
  Heap.hh
  Instant.cc
  Instant.hh
  IteratorBase.hh
  Json.cc
  Json.hh
  List.hh
  LockGuard.hh
  Log.cc
  Log.hh
  Map.hh
  Mat3.cc
  Mat3.hh
  Mat4.cc
  Mat4.hh
  Math.cc
  Math.hh
  Monitor.cc
  Monitor.hh
  Pepper.cc
  Pepper.hh
  Plane.cc
  Plane.hh
  Point.cc
  Point.hh
  Pool.cc
  Pool.hh
  Profiler.cc
  Profiler.hh
  Quat.cc
  Quat.hh
  RWLock.cc
  RWLock.hh
  Range.hh
  SBitset.hh
  SList.hh
  Semaphore.cc
  Semaphore.hh
  Set.hh
  SharedLib.cc
  SharedLib.hh
  SpinLock.cc
  SpinLock.hh
  StackTrace.cc
  StackTrace.hh
  Stream.cc
  Stream.hh
  String.cc
  String.hh
  System.cc
  System.hh
  Thread.cc
  Thread.hh
  Time.cc
  Time.hh
  Vec3.cc
  Vec3.hh
  Vec4.cc
  Vec4.hh
  common.hh
  ozCore.hh
  simd.hh
#END SOURCES
)

if(EMSCRIPTEN)
  target_link_libraries(ozCore ${PHYSFS_LIBRARY})
elseif(WIN32)
  target_link_libraries(ozCore ${PHYSFS_LIBRARY} ZLIB::ZLIB -lwinmm)
else()
  target_link_libraries(ozCore ${PHYSFS_LIBRARY} ZLIB::ZLIB -ldl ALSA::ALSA Threads::Threads)
endif()

set_target_properties(ozCore PROPERTIES VERSION ${CMAKE_PROJECT_VERSION} SOVERSION 0)

install(TARGETS ozCore RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                       LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                       ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozCore)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozCore.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
