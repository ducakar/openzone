if(NOT OZ_TOOLS)
  return()
endif()

configure_file(ozFactory.pc.in ozFactory.pc @ONLY)

add_library(ozFactory
#BEGIN SOURCES
  ImageBuilder.cc
  ImageBuilder.hh
  ImageData.cc
  ImageData.hh
  ModelBuilder.cc
  ModelBuilder.hh
  TerraBuilder.cc
  TerraBuilder.hh
  common.hh
  ozFactory.hh
#END SOURCES
)

target_compile_options(ozFactory PRIVATE -fexceptions)
target_link_libraries(ozFactory ozCore assimp::assimp ${FREEIMAGE_LIBRARY} ${NOISE_LIBRARY}
                                ${SQUISH_LIBRARY})

set_target_properties(ozFactory PROPERTIES VERSION ${CMAKE_PROJECT_VERSION} SOVERSION 0)

install(TARGETS ozFactory RUNTIME DESTINATION bin${OZ_BINARY_SUBDIR}
                          LIBRARY DESTINATION lib${OZ_BINARY_SUBDIR}
                          ARCHIVE DESTINATION lib${OZ_BINARY_SUBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN *.hh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ozFactory.pc DESTINATION lib${OZ_BINARY_SUBDIR}/pkgconfig)
