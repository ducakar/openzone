/*
 *  base.h
 *
 *  Common include file.
 *  It is included via "precompiled.h" for faster compiling.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "configuration.h"

#include <cassert>

/*
 * Basic definitions and template functions
 */
#include "common.h"
#include "iterable.h"
#include "array.h"

/*
 * Containers, part 1
 */
//#include "Tuple.h"
#include "Pair.h"
#include "List.h"
#include "DList.h"

/*
 * Memory managment
 */
#include "Alloc.h"
#include "Pool.h"

/*
 * Containers, part 2 (need memory allocation)
 */
#include "Array.h"
#include "DArray.h"
#include "Vector.h"
#include "SVector.h"
#include "Sparse.h"

/*
 * String
 */
#include "String.h"

/*
 * Containers, part 3 (need memory allocation + String)
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

/*
 * Exception class
 */
#include "Exception.h"
