/*
 *  base.h
 *
 *  Common include file.
 *  It is included in "precompiled.h", so include the latter instead of this file for faster
 *  compiling.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#ifdef WIN32
# include <boost/typeof/typeof.hpp>
# define typeof( x ) BOOST_TYPEOF( x )
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <assert.h>

/*
 * Basic definitons and template functions
 */
#include "common.hpp"

/*
 * Containers 1
 */
#include "List.hpp"
#include "DList.hpp"

#include "Vector.hpp"
#include "SVector.hpp"
#include "Sparse.hpp"

/*
 * Memory managment
 */
#include "ReuseAlloc.hpp"
#include "PoolAlloc.hpp"

/*
 * String
 */
#include "String.hpp"

/*
 * Containers 2
 */
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
#include "LogFile.hpp"
#include "Config.hpp"
