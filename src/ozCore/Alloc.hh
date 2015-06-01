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
 * @file ozCore/Alloc.hh
 *
 * `Alloc` class and enhanced `new`/`delete` operators.
 *
 * Besides `Alloc` class, enhanced `new`/`delete` operators are defined in this module, overriding
 * the standard ones.
 * - When compiled with `OZ_ALLOCATOR` the `new`/`delete` overloads also rewrite the freed memory
 *   with 0xee bytes and track memory statistics and allocated chunks to catch any memory leaks and
 *   `new`/`delete` mismatches. `Alloc::objectCIter()` and `Alloc::arrayCIter()` can be used to
 *   iterate over the chunks currently allocated by `new` and `new[]` operator respectively.
 * - If compiled with `OZ_SIMD` the allocated chunks are aligned to 16 bytes.
 *
 * @note
 * Enabling AddressSanitizer, LeakSanitizer & similar tools overrides `new`/`delete` once again and
 * hence disables the enhanced implementations.
 */

#pragma once

#include "Chain.hh"
#include "StackTrace.hh"

namespace oz
{

/**
 * Auxiliary class for custom `new`/`delete` operators.
 *
 * @sa ozCore/Alloc.hh
 */
class Alloc
{
public:

  /**
   * Information about an allocated memory chunk.
   *
   * Internal list of all memory allocations is held to detect memory leaks and `new`/`delete`
   * mismatches.
   */
  struct ChunkInfo
  {
    ChunkInfo* next[1];    ///< Pointer to the next chunk.
    size_t     size;       ///< Size (including meta data).
    StackTrace stackTrace; ///< Stack trace for the `new` call that allocated it.
  };

  /**
   * %Iterator over memory chunks allocated via overloaded `new` operators.
   */
  typedef typename Chain<ChunkInfo>::CIterator CIterator;

public:

  static int    count;     ///< Current number of allocated memory chunks.
  static size_t amount;    ///< Amount of currently allocated memory.

  static int    sumCount;  ///< Number of all memory allocations.
  static size_t sumAmount; ///< Cumulative amount of all memory allocations.

  static int    maxCount;  ///< Top number to memory allocations.
  static size_t maxAmount; ///< Top amount of allocated memory.

public:

  /**
   * Forbid instances.
   */
  Alloc() = delete;

  /**
   * Create iterator for iterating over allocated objects.
   */
  static CIterator objectCIter();

  /**
   * Create iterator for iterating over allocated arrays.
   */
  static CIterator arrayCIter();

  /**
   * Align to the previous boundary.
   */
  OZ_ALWAYS_INLINE
  static constexpr size_t alignDown(size_t size, size_t alignment = OZ_ALIGNMENT)
  {
    return size & ~(alignment - 1);
  }

  /**
   * Align to the next boundary.
   */
  OZ_ALWAYS_INLINE
  static constexpr size_t alignUp(size_t size, size_t alignment = OZ_ALIGNMENT)
  {
    return (size + alignment - 1) & ~(alignment - 1);
  }

  /**
   * Align to the previous boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type* alignDown(Type* p, size_t alignment = OZ_ALIGNMENT)
  {
    return reinterpret_cast<Type*>(size_t(p) & ~(alignment - 1));
  }

  /**
   * Align to the next boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type* alignUp(Type* p, size_t alignment = OZ_ALIGNMENT)
  {
    return reinterpret_cast<Type*>(size_t(p + alignment - 1) & ~(alignment - 1));
  }

};

}
