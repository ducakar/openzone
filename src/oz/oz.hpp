/*
 *  oz.hpp
 *
 *  Common include file for the OpenZone library (liboz).
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
 * Platform-independent argument pulling for variable-argument functions
 */
#include <cstdarg>

/*
 * assert macro
 */
#include <cassert>

/*
 * Standard math header, if we don't use built-in math
 */
#ifndef OZ_BUILTIN_MATH
# include <cmath>
#endif

/*
 * Standard exception definitions (usually included via <new>, but just for sure)
 */
#include <exception>

/*
 * Standard new/delete operator and exception definitions
 */
#include <new>

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
 * Advanced containers (HashString needs String defined)
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
 * IO
 */
#include "stream.hpp"
#include "Buffer.hpp"

/*
 * Utilities
 */
#include "Log.hpp"
#include "Config.hpp"
