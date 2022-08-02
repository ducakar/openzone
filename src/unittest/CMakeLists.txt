if(NOT OZ_TESTS)
  return()
endif()

add_executable(unittest
#BEGIN SOURCES
  Arrays.cc
  common.cc
  iterables.cc
  unittest.cc
  unittest.hh
#END SOURCES
)
target_link_libraries(unittest ozCore)

if(NACL)
  add_custom_command(OUTPUT unittest.nmf unittest.html
    DEPENDS ${CMAKE_SOURCE_DIR}/etc/nacl/unittest.nmf ${CMAKE_SOURCE_DIR}/etc/nacl/unittest.html
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/etc/nacl/unittest.nmf unittest.nmf
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/etc/nacl/unittest.html unittest.html)
  add_custom_target(unittest_files DEPENDS unittest.nmf unittest.html)

  add_dependencies(unittest unittest_files)
  set_target_properties(unittest PROPERTIES OUTPUT_NAME unittest${PLATFORM_EXE_SUFFIX})
  pnacl_finalise(unittest)
endif()

add_test(unittest unittest)
