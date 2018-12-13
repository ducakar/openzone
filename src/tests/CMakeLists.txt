if(NOT OZ_TESTS OR PLATFORM_EMBEDDED)
  return()
endif()

add_executable(containers containers.cc)
target_link_libraries(containers ozCore)

if(NOT OZ_GL_ES)
  add_executable(engine engine.cc)
  target_link_libraries(engine ozEngine)
endif()

add_executable(foreach foreach.cc)
target_link_libraries(foreach ozCore)

if(NOT OZ_GL_ES AND OZ_TOOLS)
  add_executable(noise noise.cc)
  target_link_libraries(noise ozCore ozEngine ozFactory)
endif()

add_executable(opustest opus.cc)
target_link_libraries(opustest ozEngine ozCore)

add_executable(quicksort quicksort.cc)
target_link_libraries(quicksort ozCore)

add_executable(scratch scratch.cc Foo.hh)
target_link_libraries(scratch ozCore)

add_executable(simd simd.cc)
target_link_libraries(simd ozCore)

add_executable(timespec timespec.cc)
target_link_libraries(timespec ozCore)
