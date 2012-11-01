#!/bin/sh

cat << EOF > CMakeLists.txt
configure_file( ozconfig.hh.in ozconfig.hh )

include_directories( \${CMAKE_CURRENT_BINARY_DIR} )

add_library( oz
  `echo *.{hh,cc} | sed 's| |\n  |g'` )
set_target_properties( oz PROPERTIES VERSION \${OZ_VERSION} SOVERSION 0 )

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
         RUNTIME DESTINATION bin COMPONENT liboz
         LIBRARY DESTINATION lib COMPONENT liboz
         ARCHIVE DESTINATION lib COMPONENT liboz )
install( DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include COMPONENT liboz
         FILES_MATCHING PATTERN *.hh )
install( FILES \${CMAKE_CURRENT_BINARY_DIR}/ozconfig.hh DESTINATION include/oz COMPONENT liboz )
install( FILES COPYING DESTINATION share/doc/liboz-\${OZ_VERSION} COMPONENT liboz )
EOF
