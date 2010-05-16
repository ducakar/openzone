/*
 *  oz.h
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
#include "configuration.h"

#include <cassert>

/*
 * Basic definitions and template functions
 */
#include "common.h"
#include "iterables.h"
#include "arrays.h"

/*
 * Simple containers
 */
#include "Pair.h"
#include "List.h"
#include "DList.h"
#include "Array.h"
#include "DArray.h"

/*
 * Memory management
 */
#include "Alloc.h"
#include "Pool.h"

/*
 * String
 */
#include "String.h"

/*
 * Advanced containers (need memory management and String defined)
 */
#include "Vector.h"
#include "SVector.h"
#include "Sparse.h"
#include "Map.h"
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
