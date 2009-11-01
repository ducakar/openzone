/*
 *  base.h
 *
 *  Common include file.
 *  It is included via "precompiled.h" for faster compiling.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

// defining NDEBUG disables assert macro
// #define NDEBUG

#define OZ_USE_REUSEALLOC 1
#define OZ_USE_POOLALLOC

#include <cassert>

/*
 * Basic definitons and template functions
 */
#include "common.h"
#include "iterable.h"
#include "array.h"

/*
 * Exception class
 */
#include "Exception.h"

/*
 * Containers, part 1
 */
#include "List.h"
#include "DList.h"

#include "Array.h"
#include "DArray.h"
#include "Vector.h"
#include "SVector.h"
#include "Sparse.h"

/*
 * Memory managment
 */
#include "Pool.h"
#include "PoolAlloc.h"
#include "ReuseAlloc.h"

/*
 * String
 */
#include "String.h"

/*
 * Containers, part 2 (those need String and memory management defined)
 */
#include "HashIndex.h"
#include "HashString.h"

/*
 * Bitset
 */
#include "Bitset.h"

/*
 * Math
 */
#include "Math.h"

#include "Vec3.h"
#include "Quat.h"
#include "Mat33.h"
#include "Mat44.h"

/*
 * Utilities
 */
#include "Log.h"
#include "Config.h"
