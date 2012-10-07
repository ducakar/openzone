/*
 * liboz - OpenZone Core Library.
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
 * @file oz/Alloc.hh
 *
 * Alloc class and new/delete operator overloads.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Memory allocator.
 *
 * This class implements `new`/`delete` operators that provide memory allocation statistics,
 * specific alignment and optionally track allocated chunks (if `OZ_TRACK_ALLOCS` configuration
 * option is turned on).
 *
 * Unless `NDEBUG` macro was defined when building liboz, all freed memory is rewritten with 0xee
 * byte values.
 */
class Alloc
{
  public:

    /// Alignment of allocated storage returned by `operator new`.
    static const size_t ALIGNMENT = 16;

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
     * Align to the previous boundary.
     */
    OZ_ALWAYS_INLINE
    static size_t alignDown( size_t size )
    {
      return size & ~( ALIGNMENT - 1 );
    }

    /**
     * Align to the next boundary.
     */
    OZ_ALWAYS_INLINE
    static size_t alignUp( size_t size )
    {
      return ( ( size - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT;
    }

    /**
     * Align to the previous boundary.
     */
    template <typename Type>
    OZ_ALWAYS_INLINE
    static Type* alignDown( Type* p )
    {
      return reinterpret_cast<Type*>( size_t( p ) & ~( ALIGNMENT - 1 ) );
    }

    /**
     * Align to the next boundary.
     */
    template <typename Type>
    OZ_ALWAYS_INLINE
    static Type* alignUp( Type* p )
    {
      return reinterpret_cast<Type*>( ( size_t( p - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT );
    }

    /**
     * Print summary about memory usage using the global log instance.
     */
    static void printSummary();

    /**
     * Print memory leaks using the global log.
     */
    static bool printLeaks();

};

}

/**
 * Operator new overload with memory statistics and (optionally) memory leak checking.
 *
 * Apart from standard new operator this one counts memory allocation and amount of memory allocated
 * (`Alloc::count`, `Alloc::amount` etc.). If compiled with `OZ_TRACK_ALLOCS` is also tracks all
 * allocated chunks, so it catches new/delete mismatches and `Alloc::printLeaks()` can be called at
 * any time to print currently allocated chunks.
 */
// #if defined( OZ_GCC ) && OZ_GCC < 407
// extern void* operator new ( size_t size ) _GLIBCXX_THROW( std::bad_alloc );
// #else
extern void* operator new ( size_t size );
// #endif

/**
 * Operator new[] overload with memory statistics and (optionally) memory leak checking.
 *
 * Apart from standard new operator this one counts memory allocation and amount of memory allocated
 * (`Alloc::count`, `Alloc::amount` etc.). If compiled with `OZ_TRACK_ALLOCS` is also tracks all
 * allocated chunks, so it catches new/delete mismatches and `Alloc::printLeaks()` can be called at
 * any time to print currently allocated chunks.
 */
// #if defined( OZ_GCC ) && OZ_GCC < 407
// extern void* operator new[] ( size_t size ) _GLIBCXX_THROW( std::bad_alloc );
// #else
extern void* operator new[] ( size_t size );
// #endif

/**
 * Operator delete overload with memory statistics and (optionally) memory leak checking.
 *
 * If compiled without `NDEBUG` it overwrites deallocated chunks of memory with 0xee bytes.
 */
extern void operator delete ( void* ptr ) noexcept;

/**
 * Operator delete[] overload with memory statistics and (optionally) memory leak checking.
 *
 * If compiled without `NDEBUG` it overwrites deallocated chunks of memory with 0xee bytes.
 */
extern void operator delete[] ( void* ptr ) noexcept;

/**
 * Operator new overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 */
extern void* operator new ( size_t size, const std::nothrow_t& ) noexcept;

/**
 * Operator new[] overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 */
extern void* operator new[] ( size_t size, const std::nothrow_t& ) noexcept;

/**
 * Operator delete overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 */
extern void operator delete ( void* ptr, const std::nothrow_t& ) noexcept;

/**
 * Operator delete[] overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 */
extern void operator delete[] ( void* ptr, const std::nothrow_t& ) noexcept;
