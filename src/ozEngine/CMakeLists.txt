configure_file( config.hh.in config.hh @ONLY )
configure_file( ozEngine.pc.in ozEngine.pc @ONLY )

add_library( ozEngine
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  ALBuffer.hh
  AL.hh
  ALSource.hh
  ALStreamingBuffer.hh
  common.hh
  Cursor.hh
  GL.hh
  GLTexture.hh
  ozEngine.hh
  pch.hh
  Pepper.hh
  Shader.hh
  Window.hh
  ALBuffer.cc
  AL.cc
  ALSource.cc
  ALStreamingBuffer.cc
  common.cc
  Cursor.cc
  GL.cc
  GLTexture.cc
  Pepper.cc
  Shader.cc
  Window.cc
#END SOURCES
)

add_pch( ozEngine_pch pch.hh pch.cc )
use_pch( ozEngine ozEngine_pch )

target_link_libraries( ozEngine ozCore )

# OpenGL.
if( OZ_GL_ES )
  target_link_libraries( ozEngine ${GLES2_LIBRARIES} )
else()
  target_link_libraries( ozEngine ${OPENGL_gl_LIBRARY} )
endif()

# OpenAL.
target_link_libraries( ozEngine ${OPENAL_LIBRARY} )

# SDL.
if( OZ_SDL2 )
  target_link_libraries( ozEngine ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} )
  if( OZ_NET )
    target_link_libraries( ozEngine ${SDL2_NET_LIBRARIES} )
  endif()
else()
  target_link_libraries( ozEngine ${SDL_LIBRARIES} ${SDL_TTF_LIBRARIES} )
  if( OZ_NET )
    target_link_libraries( ozEngine ${SDL_NET_LIBRARIES} )
  endif()
endif()

# libvorbisfile.
target_link_libraries( ozEngine ${VORBISFILE_LIBRARIES} )

# Dependencies.
if( PLATFORM_EMBEDDED )
  target_link_libraries( ozEngine ${FREETYPE_LIBRARY} ${VORBIS_LIBRARIES} ${OGG_LIBRARIES} )
endif()

set_target_properties( ozEngine PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

install( TARGETS ozEngine
         RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR} )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozEngine )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozEngine.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig )
