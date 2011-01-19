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
 * Bitset
 */
#include "Bitset.hpp"
#include "SBitset.hpp"

/*
 * String
 */
#include "String.hpp"

/*
 * Exception
 */
#include "Exception.hpp"

/*
 * Memory management
 */
#include "Alloc.hpp"
#include "Pool.hpp"
#include "AutoPtr.hpp"

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
 * Math
 */
#include "Math.hpp"
#include "Vec3.hpp"
#include "Point3.hpp"
#include "Quat.hpp"
#include "Mat44.hpp"

/*
 * IO
 */
#include "stream.hpp"
#include "Buffer.hpp"

/*
 * Utilities
 */
#include "Log.hpp"
#include "Config.hpp"
