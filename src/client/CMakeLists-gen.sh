#!/bin/sh

cat << EOF > CMakeLists.txt
add_library( client STATIC
  `echo *.{hh,cc} ui/*.{hh,cc} | sed 's| |\n  |g'` )
use_pch( client pch )
if( ANDROID )
  target_link_libraries( client \${SDLTTF_LIBRARY}
                                \${FREETYPE_LIBRARY}
                                \${ANDROID_GLES2_LIBRARY}
                                \${OPENAL_LIBRARY}
                                \${VORBISFILE_LIBRARY}
                                \${VORBIS_LIBRARY}
                                \${OGG_LIBRARY} )
elseif( NACL )
  target_link_libraries( client \${SDLTTF_LIBRARY}
                                \${FREETYPE_LIBRARY}
                                \${PEPPER_GLES2_LIBRARY}
                                \${OPENAL_LIBRARY}
                                \${VORBISFILE_LIBRARY}
                                \${VORBIS_LIBRARY}
                                \${OGG_LIBRARY}
                                \${PEPPER_CXX_LIBRARY}
                                \${PEPPER_LIBRARY} )
else()
  target_link_libraries( client \${SDLTTF_LIBRARY}
                                \${OPENGL_gl_LIBRARY}
                                \${OPENAL_LIBRARY}
                                \${VORBISFILE_LIBRARY} )
endif()
EOF
