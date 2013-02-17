configure_file( config.hh.in config.hh @ONLY )
configure_file( ozEngine.pc.in ozEngine.pc @ONLY )

add_library( ozEngine
  ${CMAKE_CURRENT_BINARY_DIR}/config.hh
#BEGIN SOURCES
  common.hh
  ozEngine.hh
  common.cc
  dummy.cc
#END SOURCES
)
target_link_libraries( ozEngine ozDynamics )
set_target_properties( ozEngine PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0 )

add_pch( ozEngine_pch pch.hh pch.cc )
use_pch( ozEngine ozEngine_pch )

install( TARGETS ozEngine
         RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
         LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
         ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR} )
install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/config.hh DESTINATION include/ozEngine )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/ozEngine.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig )
