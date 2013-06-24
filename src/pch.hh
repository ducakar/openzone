/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file common/pch.hh
 *
 * Precompiled header.
 */

#include <ozCore/ozCore.hh>
#include <ozDynamics/collision/AABB.hh>
#include <ozDynamics/collision/Bounds.hh>

#ifdef _WIN32
# include <windows.h>
// Fix M$ crap from Windows headers.
# undef ERROR
# undef PLANES
# undef near
# undef far
#endif

// Some standard C/C++ headers.
#include <climits>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
