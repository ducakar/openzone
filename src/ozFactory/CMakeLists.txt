if(NOT OZ_TOOLS)
  return()
endif()

configure_file(ozFactory.pc.in ozFactory.pc @ONLY)

add_library(ozFactory
#BEGIN SOURCES
  common.hh
  ImageBuilder.hh
  ImageData.hh
  ModelBuilder.hh
  ozFactory.hh
  TerraBuilder.hh
  ImageBuilder.cc
  ImageData.cc
  ModelBuilder.cc
  TerraBuilder.cc
#END SOURCES
)

target_link_libraries(ozFactory ozCore ${ASSIMP_LIBRARIES} ${FREEIMAGE_LIBRARY} ${NOISE_LIBRARY})
target_link_libraries(ozFactory ${SQUISH_LIBRARY})

set_target_properties(ozFactory PROPERTIES VERSION ${OZ_VERSION} SOVERSION 0)

install(TARGETS ozFactory RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                          LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                          ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozFactory.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
