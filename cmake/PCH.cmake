#
# Precompiled header (PCH) support for GCC and LLVM/Clang.
#
# Example:
#
#   add_pch(pch stable.h stable.c)
#   add_executable(test test.c)
#   use_pch(test pch)
#
# where:  - `pch` is PCH target name.
#         - `stable.h` is a header that should be precompiled.
#         - `stable.c` is a dummy module that contains only an include directive for `stable.h`.
#           It is required for dependency resolution and trigger recompilation of PCH.
#
# Notes:  - Only works for GCC and LLVM/Clang.
#         - Compiler flags are retrieved from `CMAKE_CXX_FLAGS`, `CMAKE_CXX_FLAGS_<BUILDTYPE>` and
#           `include_directories()` and `add_definitions()` commands for the current directory.
#         - Targets using PCH should not have any target-specific flags (`COMPILER_FLAGS` property).
#
# Author: Davorin Učakar <davorin.ucakar@gmail.com>
#

if(PCH_DISABLE)

  macro(add_pch pchTarget header module)
  endmacro()

  macro(use_pch target pchTarget)
  endmacro()

else()

  macro(add_pch pchTarget header module)
    # Extract CMAKE_CXX_FLAGS and CMAKE_CXX_FLAGS_XXX for the current configuration.
    string(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" buildTypeFlags)
    set(flags "-x c++-header ${CMAKE_CXX_FLAGS} ${${buildTypeFlags}}")

    # Convert string of space separated flags to a list.
    separate_arguments(flags)

    # Extract include directories set by include_directories commands.
    get_directory_property(includes INCLUDE_DIRECTORIES)
    foreach(include ${includes})
      list(APPEND flags "-I${include}")
    endforeach()

    # Extract definitions set by add_definitions commands.
    get_directory_property(defines COMPILE_DEFINITIONS)
    foreach(define ${defines})
      list(APPEND flags "-D${define}")
    endforeach()

    # Helper target to generate dependencies and trigger recompilation of the precompiled header
    # when neccessary.
    add_library(${pchTarget}_trigger STATIC "${header}" "${module}")

    set(inputHeader  "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
    set(outputHeader "${CMAKE_CURRENT_BINARY_DIR}/${header}")

    # Build PCH and copy original header to the build folder since we include PCH indirectly.
    add_custom_command(
      OUTPUT  "${outputHeader}.gch"
      DEPENDS ${pchTarget}_trigger
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${inputHeader}" "${outputHeader}"
      COMMAND "${CMAKE_COMMAND}" -E remove -f "${outputHeader}.gch"
      COMMAND "${CMAKE_CXX_COMPILER}" ${flags} -o "${outputHeader}.gch" "${inputHeader}")
    add_custom_target(${pchTarget} DEPENDS "${outputHeader}.gch")
    set_target_properties(${pchTarget} PROPERTIES OUTPUT_NAME "${outputHeader}")
  endmacro(add_pch)

  macro(use_pch target pchTarget)
    # Force inclusion of the header's copy in the output directory and the compiler will
    # automatically load its PCH instead. Only this PCH inclusion mechanism works fine in both GCC
    # and LLVM/Clang.
    get_target_property(pchHeader ${pchTarget} OUTPUT_NAME)
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS "-include ${pchHeader}")

    add_dependencies(${target} ${pchTarget})
    # Add explicit file dependencies so that changing the main PCH triggers rebuild. Just adding
    # pchTarget as a dependency for the current target is not enough when using Ninja build system.
    get_target_property(sources ${target} SOURCES)
    set_source_files_properties(${sources} PROPERTIES OBJECT_DEPENDS "${pchHeader}.gch")
  endmacro(use_pch)

endif()
