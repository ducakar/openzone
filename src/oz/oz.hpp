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
#include "Array.hpp"
#include "DArray.hpp"
#include "List.hpp"
#include "DList.hpp"

/*
 * String
 */
#include "String.hpp"

/*
 * Memory management
 */
#include "Alloc.hpp"
#include "Pool.hpp"

/*
 * Advanced containers
 */
#include "Vector.hpp"
#include "SVector.hpp"
#include "Map.hpp"
#include "HashIndex.hpp"
#include "HashString.hpp"

/*
 * Bitset
 */
#include "Bitset.hpp"
#include "SBitset.hpp"

/*
 * Exception
 */
#include "Exception.hpp"

/*
 * Math
 */
#include "Math.hpp"
#include "Vec4.hpp"
#include "Vec3.hpp"
#include "Point3.hpp"
#include "Plane.hpp"
#include "Quat.hpp"
#include "Mat44.hpp"

/*
 * IO
 */
#include "stream.hpp"
#include "Buffer.hpp"
#include "File.hpp"

/*
 * Utilities
 */
#include "System.hpp"
#include "Log.hpp"
#include "Config.hpp"
