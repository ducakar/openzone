/*
 *  oz.hpp
 *
 *  Common include file for oz library.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

/*
 * Configuration
 */
#include "configuration.hpp"

/*
 * Base C/C++ definitions (size_t, ptrdiff_t, NULL and offsetof)
 */
#include <cstddef>

/*
 * Standard C++ new/delete operator and exception definitions
 */
#include <new>

/*
 * assert and soft_assert macros
 */
#include "assert.hpp"

/*
 * Exception class
 */
#include "Exception.hpp"

/*
 * Basic definitions and template functions
 */
#include "common.hpp"

/*
 * Memory management
 */
#include "Alloc.hpp"
#include "Pool.hpp"

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

/*
 * String
 */
#include "String.hpp"

/*
 * Advanced containers (need String defined)
 */
#include "Vector.hpp"
#include "SVector.hpp"
#include "Sparse.hpp"
#include "Map.hpp"
#include "HashIndex.hpp"
#include "HashString.hpp"

/*
 * Bitset
 */
#include "Bitset.hpp"

/*
 * Math
 */
#include "Math.hpp"

#include "Vec3.hpp"
#include "Quat.hpp"
#include "Mat33.hpp"
#include "Mat44.hpp"

/*
 * Utilities
 */
#include "Log.hpp"
#include "Config.hpp"
