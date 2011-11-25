/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file stable.hpp
 *
 * Precompiled header.
 *
 * It should be included in all '.cpp' source files in the project. Do not include it via another
 * header, it will not work in GCC or VC++.
 */

#include "common/common.hpp"

// we want to use C++ wrapped C headers, not vanilla C ones that are included via SDL
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// gettext
#include <libintl.h>

// include SDL
#define SDL_NO_COMPAT
#include <SDL/SDL.h>

// fix M$ crap from Windows headers
#ifdef OZ_MINGW
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif
