# Author: Davorin Učakar <davorin.ucakar@gmail.com>

# add_syncheck( target_name "-DFOO -DBAR" file1 file2 ... )
# Notes: - Compiler flags are retrieved from CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_BUILDTYPE,
#          everything set via add_definitions() is ignored
#        - definitions is a string of space-separated additional flags for compiler.
macro( add_syncheck _targetName _definitions )
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

  # definitions are given in a string because we already use varargs for files.
  separate_arguments( _definitions )

  # set build rules (build precompiled header and copy original header to build folder - needed to
  # be there to be included via other headers - which is needed for KDevelop's indexer to work
  # properly)
  add_custom_command( OUTPUT ${_targetName}_checking
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_CXX_COMPILER} ${_flags} ${_definitions} -fsyntax-only ${ARGN} )
  add_custom_target( ${_targetName}
   DEPENDS ${_targetName}_checking )
endmacro()
