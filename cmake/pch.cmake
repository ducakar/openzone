# Author: Davorin Učakar <davorin.ucakar@gmail.com>

# add_pch( target_name stable.h stable.c )
# where: - stable.h is a header that should be precompiled
#        - stable.c is a dummy module that contains only directive to include stable.h (it is
#          required for proper dependency resolution to trigger recompilation of PCH)
# Notes: - Only works for GCC and compatible compilers (e.g. LLVM/Clang).
#        - Compiler flags are retrieved from CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_BUILDTYPE.
#          Include directories added via include_directories and defines added via add_definitions
#          are also added to compiler flags.
macro( add_pch _targetName _inputHeader _triggerModule )
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

  # helper target that properly triggers recompilation of precompiled header
  add_library( ${_targetName}_trigger STATIC "${_triggerModule}" )

  # set build rules (build precompiled header and copy original header to the build folder - needs
  # to be there to be included via other headers - which is needed for KDevelop's indexer to work
  # properly)
  add_custom_command( OUTPUT "${_inputHeader}.gch"
    DEPENDS ${_targetName}_trigger
    COMMAND ${CMAKE_COMMAND} -E remove -f "${_inputHeader}.gch"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" "${_inputHeader}"
    COMMAND ${CMAKE_CXX_COMPILER} ${_flags} -o "${_inputHeader}.gch" "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" )
  add_custom_target( ${_targetName} DEPENDS "${_inputHeader}.gch" )
endmacro()
