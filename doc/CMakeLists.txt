Find_Package( Doxygen )

add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.liboz
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    COMMAND ${DOXYGEN_EXECUTABLE} doc/Doxyfile.liboz )
add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doxygen
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    COMMAND ${DOXYGEN_EXECUTABLE} doc/Doxyfile )
add_custom_target( doc DEPENDS doxygen.liboz doxygen )
