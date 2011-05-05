/*
 *  oz.hpp
 *
 *  Common include file for the OpenZone library (liboz).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/*
 * Configuration
 */
#include "ozconfig.hpp"

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
 * Advanced containers (HashString needs String defined)
 */
#include "Vector.hpp"
#include "SVector.hpp"
#include "Map.hpp"
#include "Sparse.hpp"
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
#include "Vec4.hpp"
#include "Vec3.hpp"
#include "Point3.hpp"
#include "Plane.hpp"
#include "Quat.hpp"
#include "Mat44.hpp"

/*
 * Exception
 */
#include "Exception.hpp"

/*
 * IO
 */
#include "stream.hpp"
#include "Buffer.hpp"
#include "Directory.hpp"

/*
 * Utilities
 */
#include "System.hpp"
#include "Log.hpp"
#include "Config.hpp"
