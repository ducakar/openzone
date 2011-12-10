# Author: Davorin Učakar <davorin.ucakar@gmail.com>

# add_pch( target_name header.h DEPENDS dep1 dep2 ... )
# Notes: - Fails if the source directory is used as the build directory.
#        - Only works for GCC and compatible compilers (e.g. LLVM/Clang).
#        - Compiler flags are retrieved from CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_BUILDTYPE.
#          Include directories added via include_directories and defines added via add_definitions
#          are also added to compiler flags.
macro( add_pch _targetName _inputHeader )
  # extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for current build configuration XXX
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

  # convert string of space separated flags into a list
  separate_arguments( _flags )

  # extract include directories set by include_directories command
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _include ${_includes} )
    list( APPEND _flags "-I${_include}" )
  endforeach()

  # extract definitions set by add_definitions command
  get_directory_property( _defines COMPILE_DEFINITIONS )
  foreach( _define ${_defines} )
    list( APPEND _flags "-D${_define}" )
  endforeach()

  # set build rules (build precompiled header and copy original header to the build folder - needs
  # to be there to be included via other headers - which is needed for KDevelop's indexer to work
  # properly)
  add_custom_command( OUTPUT "${_inputHeader}.gch"
    ${ARGN} "${_inputHeader}"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${_inputHeader}" "${_inputHeader}.gch"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" "${_inputHeader}"
    COMMAND ${CMAKE_CXX_COMPILER} ${_flags} -o "${_inputHeader}.gch" "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" )
  add_custom_target( ${_targetName}
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
    DEPENDS "${_inputHeader}.gch" )
endmacro()
