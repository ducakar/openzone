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
 * @file oz/oz.hpp
 *
 * Common include file for liboz.
 *
 * @defgroup oz liboz - OpenZone core library
 */

#pragma once

/*
 * Base definitions and templates
 */
#include "common.hpp"

/*
 * Iterators and utilities for iterable containers and arrays
 */
#include "iterables.hpp"
#include "arrays.hpp"

/*
 * Simple containers
 */
#include "Pair.hpp"
#include "List.hpp"
#include "DList.hpp"
#include "Array.hpp"
#include "DArray.hpp"
#include "SVector.hpp"
#include "Vector.hpp"
#include "Map.hpp"

/*
 * Memory management
 */
#include "Alloc.hpp"
#include "Pool.hpp"

/*
 * String
 */
#include "String.hpp"

/*
 * Advanced containers
 */
#include "HashIndex.hpp"
#include "HashString.hpp"

/*
 * Bitset
 */
#include "Bitset.hpp"
#include "SBitset.hpp"

/*
 * Math
 */
#include "Math.hpp"
#include "Simd.hpp"
#include "Vec4.hpp"
#include "Vec3.hpp"
#include "Point3.hpp"
#include "Plane.hpp"
#include "Quat.hpp"
#include "Mat44.hpp"

/*
 * IO
 */
#include "Endian.hpp"
#include "InputStream.hpp"
#include "OutputStream.hpp"
#include "BufferStream.hpp"
#include "Buffer.hpp"
#include "File.hpp"

/*
 * Error handling.
 */
#include "StackTrace.hpp"
#include "Exception.hpp"
#include "System.hpp"

/*
 * Utilities
 */
#include "Log.hpp"
#include "Config.hpp"
