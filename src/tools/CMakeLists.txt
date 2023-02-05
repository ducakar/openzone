if(NACL)

  add_executable(openzone openzone.cc)
  target_precompile_headers(openzone REUSE_FROM common)
  target_link_libraries(openzone client nirvana matrix common ozEngine)
  set_target_properties(openzone PROPERTIES OUTPUT_NAME openzone${PLATFORM_EXE_SUFFIX})
  pnacl_finalise(openzone)

else()

  add_executable(openzone openzone.cc)
  target_precompile_headers(openzone REUSE_FROM common)
  target_link_libraries(openzone client nirvana matrix common ozEngine)
  set_target_properties(openzone PROPERTIES WIN32_EXECUTABLE ON)
  install(TARGETS openzone RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR})

  if(OZ_TOOLS)

    add_executable(ozBuild ozBuild.cc)
    target_precompile_headers(ozBuild REUSE_FROM common)
    target_link_libraries(ozBuild builder client nirvana matrix common ozFactory ozEngine)
    install(TARGETS ozBuild RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR})

    add_executable(ozGenEnvMap ozGenEnvMap.cc)
    target_link_libraries(ozGenEnvMap ozFactory)
    install(TARGETS ozGenEnvMap RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR})

    add_executable(ozGettext ozGettext.cc)
    target_link_libraries(ozGettext ozCore)
    install(TARGETS ozGettext RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR})

  endif()

endif()
