/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/oz.hh
 *
 * Common include file for liboz.
 */

#pragma once

/*
 * Base definitions and templates.
 */
#include "common.hh"

/*
 * Memory management.
 */
#include "Alloc.hh"
#include "Pool.hh"

/*
 * Iterators and container utilities.
 */
#include "iterables.hh"
#include "arrays.hh"

/*
 * Containers.
 */
#include "Pair.hh"
#include "Chain.hh"
#include "DChain.hh"
#include "Array.hh"
#include "DArray.hh"
#include "List.hh"
#include "SList.hh"
#include "Set.hh"
#include "Map.hh"
#include "Hashtable.hh"

/*
 * Bitset.
 */
#include "Bitset.hh"
#include "SBitset.hh"

/*
 * String.
 */
#include "String.hh"

/*
 * Math.
 */
#include "simd.hh"
#include "Math.hh"
#include "Vec3.hh"
#include "Point.hh"
#include "Plane.hh"
#include "Vec4.hh"
#include "Quat.hh"
#include "Mat44.hh"

/*
 * Bounding volumes.
 */
#include "AABB.hh"
#include "Bounds.hh"

/*
 * System.
 */
#include "Time.hh"
#include "StackTrace.hh"
#include "System.hh"

/*
 * Threading.
 */
#include "Thread.hh"
#include "Mutex.hh"
#include "Semaphore.hh"

/*
 * I/O.
 */
#include "Endian.hh"
#include "InputStream.hh"
#include "OutputStream.hh"
#include "BufferStream.hh"
#include "Buffer.hh"
#include "File.hh"
#include "PFile.hh"

/*
 * Utilities.
 */
#include "Log.hh"
#include "JSON.hh"
