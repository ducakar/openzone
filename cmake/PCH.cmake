# Author: Davorin Učakar <davorin.ucakar@gmail.com>

#
# Precompiled header (PCH) support for GCC and LLVM/Clang.
#
# Example:
#
#   add_pch( pch stable.h stable.c )
#   add_executable( test test.c )
#   use_pch( test pch )
#
# where: - `pch` is PCH target name.
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

  macro( add_pch pchTarget header module )
    # Extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for the current configuration XXX.
    string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" buildTypeFlagsVar )
    set( flags "${CMAKE_CXX_FLAGS} ${${buildTypeFlagsVar}}" )

    # Convert string of space separated flags into a list.
    separate_arguments( flags )

    # Extract include directories set by include_directories command.
    get_directory_property( includes INCLUDE_DIRECTORIES )
    foreach( include ${includes} )
      list( APPEND flags "-I${include}" )
    endforeach()

    # Extract definitions set by add_definitions command.
    get_directory_property( defines COMPILE_DEFINITIONS )
    foreach( define ${defines} )
      list( APPEND flags "-D${define}" )
    endforeach()

    # Helper target that properly triggers recompilation of precompiled header.
    add_library( ${pchTarget}_trigger STATIC "${header}" "${module}" )

    set( inputHeader  "${CMAKE_CURRENT_SOURCE_DIR}/${header}" )
    set( outputHeader "${CMAKE_CURRENT_BINARY_DIR}/${header}" )

    # Build PCH and copy original header to the build folder since we include PCH indirectly.
    add_custom_command(
      OUTPUT  "${outputHeader}.gch"
      DEPENDS ${pchTarget}_trigger
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${inputHeader}" "${outputHeader}"
      COMMAND "${CMAKE_COMMAND}" -E remove -f "${outputHeader}.gch"
      COMMAND "${CMAKE_CXX_COMPILER}" ${flags} -o "${outputHeader}.gch" "${inputHeader}" )
    add_custom_target( ${pchTarget} DEPENDS "${outputHeader}.gch" )
    set_target_properties( ${pchTarget} PROPERTIES OUTPUT_NAME "${outputHeader}" )
  endmacro( add_pch )

  macro( use_pch target pchTarget )
    add_dependencies( ${target} ${pchTarget} )
    get_target_property( pchHeader ${pchTarget} OUTPUT_NAME )
    set_target_properties( ${target} PROPERTIES COMPILE_FLAGS "-include ${pchHeader}" )

    # Add explicit dependencies. This is required when building with Ninja, otherwise modules won't
    # get recompiled whenever a header that is also included via PCH is changed.
    get_target_property( sources ${target} SOURCES )
    set_source_files_properties( ${sources} PROPERTIES OBJECT_DEPENDS "${pchHeader}.gch" )
  endmacro( use_pch )

endif( PCH_DISABLE )
