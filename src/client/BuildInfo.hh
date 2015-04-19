/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 */

/**
 * @file BuildInfo.hh
 *
 * The .cc file is auto-generated on each build and provides information about the current build.
 */

#pragma once

#include <client/common.hh>

namespace oz
{

struct BuildInfo
{
  /// Data and time.
  static const char* const TIME;

  /// Host system.
  static const char* const HOST;

  /// Target architecture.
  static const char* const HOST_ARCH;

  /// Target architecture.
  static const char* const TARGET_ARCH;

  /// Build configuration.
  static const char* const BUILD_TYPE;

  /// Compiler executable and version.
  static const char* const COMPILER;

  /// Compiler flags.
  static const char* const CXX_FLAGS;

  /// Configuration options.
  static const char* const CONFIG;
};

}
