/*
 *  oz.hh
 *
 *  Common include file for oz library.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

/*
 * Configuration
 */
#include "configuration.hh"

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
#include "assert.hh"

/*
 * Exception class
 */
#include "Exception.hh"

/*
 * Basic definitions and template functions
 */
#include "common.hh"

/*
 * Memory management
 */
#include "Alloc.hh"
#include "Pool.hh"

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

/*
 * String
 */
#include "String.hh"

/*
 * Advanced containers (need String defined)
 */
#include "Vector.hh"
#include "SVector.hh"
#include "Sparse.hh"
#include "Map.hh"
#include "HashIndex.hh"
#include "HashString.hh"

/*
 * Bitset
 */
#include "Bitset.hh"

/*
 * Math
 */
#include "Math.hh"

#include "Vec3.hh"
#include "Quat.hh"
#include "Mat33.hh"
#include "Mat44.hh"

/*
 * Utilities
 */
#include "Log.hh"
#include "Config.hh"

/*
 * Imports oz namespace and explicitly imports all symbols from oz that may be ambiguous
 */
#define OZ_IMPORT() \
  using namespace oz; \
  using oz::min; \
  using oz::max; \
  using oz::abs; \
  using oz::log;
