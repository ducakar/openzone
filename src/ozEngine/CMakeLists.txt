configure_file(config.hh.in config.hh @ONLY)
configure_file(ozEngine.pc.in ozEngine.pc @ONLY)

add_library(ozEngine
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  AL.cc
  AL.hh
  Application.cc
  Application.hh
  Cursor.cc
  Cursor.hh
  Font.cc
  Font.hh
  GL.cc
  GL.hh
  Input.cc
  Input.hh
  Lua.cc
  Lua.hh
  Window.cc
  Window.hh
  common.hh
  ozEngine.hh
#END SOURCES
)

if(EMSCRIPTEN)
  target_link_libraries(ozEngine ${OPUSFILE_LIBRARIES} ${OPUS_LIBRARIES})
else()
  target_link_libraries(ozEngine $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
                                 $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
                                 ${GL_LIBRARIES} OpenAL::OpenAL PNG::PNG
                                 ${OPUSFILE_LIBRARIES} ${OPUS_LIBRARIES} ${VORBISFILE_LIBRARIES})
endif()
target_link_libraries(ozEngine ${LUA_LIBRARIES} ozCore)

set_target_properties(ozEngine PROPERTIES VERSION ${CMAKE_PROJECT_VERSION} SOVERSION 0)

install(TARGETS ozEngine RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozEngine)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozEngine.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
