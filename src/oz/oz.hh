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
 * @file oz/oz.hh
 *
 * Common include file for liboz.
 *
 * @defgroup oz liboz - OpenZone core library
 */

#pragma once

/*
 * Base definitions and templates
 */
#include "common.hh"

/*
 * Iterators and utilities for iterable containers and arrays
 */
#include "iterables.hh"
#include "arrays.hh"

/*
 * Simple containers
 */
#include "Pair.hh"
#include "List.hh"
#include "DList.hh"
#include "Array.hh"
#include "DArray.hh"
#include "SVector.hh"
#include "Vector.hh"
#include "Map.hh"

/*
 * Memory management
 */
#include "Alloc.hh"
#include "Pool.hh"

/*
 * String
 */
#include "String.hh"

/*
 * Advanced containers
 */
#include "HashIndex.hh"
#include "HashString.hh"

/*
 * Bitset
 */
#include "Bitset.hh"
#include "SBitset.hh"

/*
 * Math
 */
#include "Math.hh"
#include "Simd.hh"
#include "Vec4.hh"
#include "Vec3.hh"
#include "Point3.hh"
#include "Plane.hh"
#include "Quat.hh"
#include "Mat44.hh"

/*
 * IO
 */
#include "Endian.hh"
#include "InputStream.hh"
#include "OutputStream.hh"
#include "BufferStream.hh"
#include "Buffer.hh"
#include "File.hh"

/*
 * Error handling.
 */
#include "StackTrace.hh"
#include "Exception.hh"
#include "System.hh"

/*
 * Utilities
 */
#include "Log.hh"
#include "Config.hh"
