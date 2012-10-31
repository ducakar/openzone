#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( ozconfig.hh.in ozconfig.hh )

include_directories( \${CMAKE_CURRENT_BINARY_DIR} )

set( headers
  \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh
  `echo *.hh | sed 's| |\n  |g'` )
add_library( oz
  `echo *.{hh,cc} | sed 's| |\n  |g'` )
set_target_properties( oz PROPERTIES
                       PUBLIC_HEADER "\${headers}"
                       VERSION \${OZ_VERSION}
                       SOVERSION 0 )

if( ANDROID )
  target_link_libraries( oz \${PLATFORM_STL_LIBRARY}
                            \${ANDROID_LOG_LIBRARY}
                            \${OPENSLES_LIBRARY}
                            \${PHYSFS_LIBRARY}
                            \${ZLIB_LIBRARY} )
elseif( NACL )
  target_link_libraries( oz \${PTHREAD_LIBRARY}
                            \${PHYSFS_LIBRARY}
                            \${ZLIB_LIBRARY}
                            \${PEPPER_CXX_LIBRARY}
                            \${PEPPER_LIBRARY} )
elseif( WIN32 )
  target_link_libraries( oz \${WINMM_LIBRARY}
                            \${PHYSFS_LIBRARY} )
else()
  target_link_libraries( oz \${PTHREAD_LIBRARY}
                            \${RT_LIBRARY}
                            \${DL_LIBRARY}
                            \${ALSA_LIBRARY}
                            \${PHYSFS_LIBRARY} )
endif()

install( TARGETS oz
         RUNTIME DESTINATION bin
         LIBRARY ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include/oz )
EOF
