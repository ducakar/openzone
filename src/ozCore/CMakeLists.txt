configure_file(config.hh.in config.hh @ONLY)
configure_file(ozCore.pc.in ozCore.pc @ONLY)

add_library(ozCore
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  Alloc.hh
  arrays.hh
  Bitset.hh
  Buffer.hh
  CallOnce.hh
  Chain.hh
  common.hh
  DChain.hh
  Endian.hh
  EnumMap.hh
  File.hh
  Gettext.hh
  HashMap.hh
  HashSet.hh
  InputStream.hh
  iterables.hh
  Java.hh
  Json.hh
  List.hh
  Log.hh
  Map.hh
  Mat3.hh
  Mat4.hh
  Math.hh
  Mutex.hh
  OutputStream.hh
  ozCore.hh
  Pair.hh
  Pepper.hh
  Plane.hh
  Point.hh
  Pool.hh
  Profiler.hh
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
  arrays.cc
  Bitset.cc
  Buffer.cc
  CallOnce.cc
  common.cc
  EnumMap.cc
  File.cc
  Gettext.cc
  InputStream.cc
  Java.cc
  Json.cc
  Log.cc
  Mat3.cc
  Mat4.cc
  Math.cc
  Mutex.cc
  OutputStream.cc
  Pepper.cc
  Plane.cc
  Point.cc
  Pool.cc
  Profiler.cc
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

target_link_libraries(ozCore ${PHYSFS_LIBRARY} ${ZLIB_LIBRARY})
if(ANDROID)
  target_link_libraries(ozCore OpenSLES -ldl -llog ${PLATFORM_STL_LIBRARY})
elseif(NACL)
  target_link_libraries(ozCore -lppapi_cpp -lppapi -lpthread)
elseif(WIN32)
  target_link_libraries(ozCore -lwinmm)
elseif(NOT LINUX)
  target_link_libraries(ozCore -ldl -lrt -lpthread)
else()
  target_link_libraries(ozCore ${ALSA_LIBRARIES} -ldl -lrt -lpthread)
endif()

set_target_properties(ozCore PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0)

install(TARGETS ozCore RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                       LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                       ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozCore)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozCore.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
