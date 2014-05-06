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
 * Apart form `Alloc` class, enhanced `new`/`delete` operators are defined in this module,
 * overriding standard ones defined in \<new\>. Those operators count number of allocations and
 * amount of memory allocated at any given time (reported through `Alloc::count`, `Alloc::amount`
 * etc.).
 *
 * Several other features may also be configured:
 * - Unless compiled with `NDEBUG` all freed memory is overwritten by 0xEE bytes.
 * - If compiled with `OZ_TRACK_ALLOCS` `new`/`delete` also tracks allocated chunks to catch memory
 *   leaks and `new`/`delete` mismatches. `Alloc::objectCIter()` and `Alloc::arrayCIter()` can be
 *   used to iterate over chunks allocated by `new` and `new[]` operators respectively.
 * - If compiled with `OZ_SIMD_MATH` allocated chunks are aligned to size of 4 floats.
 *
 * @note
 * Enabling AddressSanitizer during compilation of liboz disables enhanced `new`/`delete` operator
 * implementations.
 */

#pragma once

#include "iterables.hh"
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
    ChunkInfo*  next;       ///< Pointer to the next chunk.
    void*       address;    ///< Address of actual data (meta data lays before this address).
    size_t      size;       ///< Size (including meta data).
    StackTrace  stackTrace; ///< Stack trace for the `new` call that allocated it.
  };

  /**
   * %Iterator for memory chunks allocated via overloaded `new` and `new[]` operators.
   */
  class ChunkCIterator : public IteratorBase<const ChunkInfo>
  {
  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    explicit ChunkCIterator() :
      IteratorBase<const ChunkInfo>( nullptr )
    {}

    /**
     * Create iterator for a given `ChunkInfo` element (used internally).
     */
    OZ_ALWAYS_INLINE
    explicit ChunkCIterator( const ChunkInfo* chunkInfo ) :
      IteratorBase<const ChunkInfo>( chunkInfo )
    {}

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    ChunkCIterator& operator ++ ()
    {
      hard_assert( elem != nullptr );

      elem = elem->next;
      return *this;
    }

    /**
     * STL-style begin iterator.
     */
    OZ_ALWAYS_INLINE
    ChunkCIterator begin() const
    {
      return *this;
    }

    /**
     * STL-style end iterator.
     */
    OZ_ALWAYS_INLINE
    ChunkCIterator end() const
    {
      return ChunkCIterator();
    }

  };

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
  explicit Alloc() = delete;

  /**
   * Create iterator for iterating over allocated objects.
   */
  static ChunkCIterator objectCIter();

  /**
   * Create iterator for iterating over allocated arrays.
   */
  static ChunkCIterator arrayCIter();

  /**
   * Align to the previous boundary.
   */
  OZ_ALWAYS_INLINE
  static constexpr size_t alignDown( size_t size )
  {
    return size & ~( OZ_ALIGNMENT - 1 );
  }

  /**
   * Align to the next boundary.
   */
  OZ_ALWAYS_INLINE
  static constexpr size_t alignUp( size_t size )
  {
    return ( size + OZ_ALIGNMENT - 1 ) & ~( OZ_ALIGNMENT - 1 );
  }

  /**
   * Align to the previous boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type* alignDown( Type* p )
  {
    return reinterpret_cast<Type*>( size_t( p ) & ~( OZ_ALIGNMENT - 1 ) );
  }

  /**
   * Align to the next boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type* alignUp( Type* p )
  {
    return reinterpret_cast<Type*>( size_t( p + OZ_ALIGNMENT - 1 ) & ~( OZ_ALIGNMENT - 1 ) );
  }

};

}
