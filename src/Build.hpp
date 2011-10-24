/*
 *  Build.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

/**
 * @file Build.hpp
 *
 * The .cpp file is auto-generated on each build and provides information about the current build.
 */

namespace oz
{

class Build
{
  public:

    /// Data and time.
    static const char* TIME;

    /// Host system.
    static const char* HOST_SYSTEM;

    /// Target system.
    static const char* TARGET_SYSTEM;

    /// Build configuration.
    static const char* BUILD_TYPE;

    /// Compiler executable and version.
    static const char* COMPILER;

    /// Compiler flags.
    static const char* CXX_FLAGS;

    /// Linker flags.
    static const char* EXE_LINKER_FLAGS;

};

}
