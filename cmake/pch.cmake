# Author: Davorin Učakar <davorin.ucakar@gmail.com>

# add_pch( target_name header.hpp DEPENDS dep1 dep2 ... )
# Notes: - Fails if the source directory is used as the build directory
#        - Only works for GCC and compatible compilers (e.g. Clang)
macro( add_pch _targetName _inputHeader )
  # extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for current build configuration XXX
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _build_type_flags_var )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_build_type_flags_var}}" )

  # convert string of space separated flags into a list
  separate_arguments( _flags )

  # extract include directories set by include_directories command
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _inc_dir ${_includes} )
    list( APPEND _flags "-I${_inc_dir}" )
  endforeach()

  # set build rules (build precompiled header and copy original header to build folder - needed to
  # be there to be included via other headers - which is needed for KDevelop's indexer to work
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
