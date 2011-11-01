/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
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
