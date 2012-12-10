# Author: Davorin Učakar <davorin.ucakar@gmail.com>

# Example:
#
#   add_pch( pch stable.h stable.c )
#   add_executable( test test.c )
#   use_pch( test pch )
#
# where: - `pch` is PCH target name
#        - `stable.h` is a header that should be precompiled.
#        - `stable.c` is a dummy module that contains only an include directive for `stable.h` (it
#          is required for proper dependency resolution to trigger recompilation of PCH).
# Notes: - Only works for GCC and LLVM/Clang.
#        - Compiler flags are retrieved from `CMAKE_CXX_FLAGS`, `CMAKE_CXX_FLAGS_<BUILDTYPE>`,
#          included directories added via `include_directories()` and defines added via
#          `add_definitions()`.
#        - If a target using PCH has some target-specific compiler flags (`COMPILER_FLAGS property),
#          those are overridden by `use_pch()` macro. Otherwise PCH would be unusable for
#          such targets anyway as PCH must be compiled with exactly the same flags as the target.

if( PCH_DISABLE )

macro( add_pch )
endmacro( add_pch )

macro( use_pch )
endmacro( use_pch )

else( PCH_DISABLE )

macro( add_pch _pchTarget _inputHeader _inputModule )
  # Extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for the current configuration XXX.
  string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _buildTypeFlagsVar )
  set( _flags "${CMAKE_CXX_FLAGS} ${${_buildTypeFlagsVar}}" )

  # Convert string of space separated flags into a list.
  separate_arguments( _flags )

  # Extract include directories set by include_directories command.
  get_directory_property( _includes INCLUDE_DIRECTORIES )
  foreach( _include ${_includes} )
    list( APPEND _flags "-I${_include}" )
  endforeach()

  # Extract definitions set by add_definitions command.
  get_directory_property( _defines COMPILE_DEFINITIONS )
  foreach( _define ${_defines} )
    list( APPEND _flags "-D${_define}" )
  endforeach()

  # Helper target that properly triggers recompilation of precompiled header.
  add_library( ${_pchTarget}_trigger STATIC "${_inputModule}" )

  # Build PCH and copy original header to the build folder since we include PCH indirectly.
  add_custom_command( OUTPUT "${_inputHeader}.gch"
    DEPENDS ${_pchTarget}_trigger
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}"
            "${_inputHeader}"
    COMMAND "${CMAKE_COMMAND}" -E remove -f "${_inputHeader}.gch"
    COMMAND "${CMAKE_CXX_COMPILER}" ${_flags} -o "${_inputHeader}.gch"
            "${CMAKE_CURRENT_SOURCE_DIR}/${_inputHeader}" )
  add_custom_target( ${_pchTarget} DEPENDS "${_inputHeader}.gch" )

  # Cache header location for later `use_pch()` macros.
  set( ${_pchTarget}_output "${CMAKE_CURRENT_BINARY_DIR}/${_inputHeader}" )
endmacro( add_pch )

macro( use_pch _target _pchTarget )
  add_dependencies( ${_target} ${_pchTarget} )

  set_target_properties( ${_target} PROPERTIES COMPILE_FLAGS "-include ${${_pchTarget}_output}" )
endmacro( use_pch )

endif( PCH_DISABLE )
