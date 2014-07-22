if(NOT OZ_TESTS)
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

add_executable(quicksort quicksort.cc)
target_link_libraries(quicksort ozCore)

add_executable(scratch scratch.cc)
target_link_libraries(scratch ozCore ozEngine)

add_executable(sdlEmu sdlEmu.cc)
target_link_libraries(sdlEmu ozCore ozEngine)

add_executable(simd simd.cc)
target_link_libraries(simd ozCore)
