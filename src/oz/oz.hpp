/*
 *  oz.hpp
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
#include "ozconfig.hpp"

/*
 * Base C/C++ definitions (size_t, ptrdiff_t, NULL and offsetof)
 */
#include <cstddef>

/*
 * assert macro
 */
#include <cassert>

/*
 * Standard C++ new/delete operator and exception definitions
 */
#include <new>

/*
 * Base definitions and templates
 */
#include "common.hpp"

/*
 * Exception class
 */
#include "Exception.hpp"

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
#include "SBitset.hpp"

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

/*
 * Imports oz namespace and explicitly imports all symbols from oz that may be ambiguous
 */
#define OZ_IMPORT() \
  using namespace oz; \
  using oz::min; \
  using oz::max; \
  using oz::abs; \
  using oz::log;
