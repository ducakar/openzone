/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/ozCore.hh
 *
 * Main public header for OpenZone Core Library.
 */

#pragma once

/*
 * Base definitions and templates.
 */
#include "common.hh"

/*
 * Iterators and container utilities.
 */
#include "iterables.hh"
#include "arrays.hh"

/*
 * System.
 */
#include "System.hh"
#include "StackTrace.hh"

/*
 * Memory management.
 */
#include "Alloc.hh"
#include "Pool.hh"

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
#include "HashSet.hh"
#include "HashMap.hh"

/*
 * Bit arrays.
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
#include "Mat3.hh"
#include "Mat4.hh"

/*
 * Threads.
 */
#include "SpinLock.hh"
#include "Mutex.hh"
#include "Semaphore.hh"
#include "CallOnce.hh"
#include "Thread.hh"

/*
 * I/O.
 */
#include "Endian.hh"
#include "InputStream.hh"
#include "OutputStream.hh"
#include "Buffer.hh"
#include "File.hh"

/*
 * Utilities.
 */
#include "Time.hh"
#include "Profiler.hh"
#include "EnumMap.hh"
#include "SharedLib.hh"
#include "Gettext.hh"
#include "JSON.hh"
#include "MainCall.hh"
#include "Log.hh"

/*
 * Platform support.
 */
#include "Java.hh"
#include "Pepper.hh"
