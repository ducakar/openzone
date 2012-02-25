/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
 * This class implements <tt>new</tt>/<tt>delete</tt> operators that provide memory allocation
 * statistics, specific alignment and optionally track memory leaks (if <tt>OZ_TRACK_LEAKS</tt>
 * configuration option is turned on).
 *
 * Unless <tt>NDEBUG</tt> macro was defined when building liboz, all freed memory is rewritten with
 * 0xee byte values.
 *
 * @ingroup oz
 */
class Alloc
{
  public:

    /// Alignment of allocated storage returned by <tt>operator new</tt>.
    static const size_t ALIGNMENT = 16;

    static int    count;     ///< Current number of allocated memory chunks.
    static size_t amount;    ///< Amount of currently allocated memory.

    static int    sumCount;  ///< Number of all memory allocations.
    static size_t sumAmount; ///< Cumulative amount of all memory allocations.

    static int    maxCount;  ///< Top number to memory allocations.
    static size_t maxAmount; ///< Top amount of allocated memory.

    /**
     * Singleton.
     */
    Alloc() = delete;

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
    static void printLeaks();

};

}

/**
 * Operator new overload with memory statistics and (optionally) memory leak checking.
 *
 * Apart from standard new operator this one counts memory allocation and amount of memory allocated
 * (<tt>Alloc::count</tt>, <tt>Alloc::amount</tt> etc.). If compiled with <tt>OZ_TRACK_LEAKS</tt>
 * is also tracks all allocated chunks, so it catches new/delete mismatches and
 * <tt>Alloc::printLeaks()</tt> can be called at any time to print currently allocated chunks.
 *
 * @ingroup oz
 */
extern void* operator new ( std::size_t size ) throw( std::bad_alloc );

/**
 * Operator new[] overload with memory statistics and (optionally) memory leak checking.
 *
 * Apart from standard new operator this one counts memory allocation and amount of memory allocated
 * (<tt>Alloc::count</tt>, <tt>Alloc::amount</tt> etc.). If compiled with <tt>OZ_TRACK_LEAKS</tt>
 * is also tracks all allocated chunks, so it catches new/delete mismatches and
 * <tt>Alloc::printLeaks()</tt> can be called at any time to print currently allocated chunks.
 *
 * @ingroup oz
 */
extern void* operator new[] ( std::size_t size ) throw( std::bad_alloc );

/**
 * Operator delete overload with memory statistics and (optionally) memory leak checking.
 *
 * If compiled without <tt>NDEBUG</tt> it overwrites deallocated chunks of memory with 0xee bytes.
 *
 * @ingroup oz
 */
extern void operator delete ( void* ptr ) throw();

/**
 * Operator delete[] overload with memory statistics and (optionally) memory leak checking.
 *
 * If compiled without <tt>NDEBUG</tt> it overwrites deallocated chunks of memory with 0xee bytes.
 *
 * @ingroup oz
 */
extern void operator delete[] ( void* ptr ) throw();

/**
 * Operator new overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 *
 * @ingroup oz
 */
extern void* operator new ( std::size_t size, const std::nothrow_t& ) throw();

/**
 * Operator new[] overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 *
 * @ingroup oz
 */
extern void* operator new[] ( std::size_t size, const std::nothrow_t& ) throw();

/**
 * Operator delete overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 *
 * @ingroup oz
 */
extern void operator delete ( void* ptr, const std::nothrow_t& ) throw();

/**
 * Operator delete[] overload with memory statistics and (optionally) memory leak checking.
 *
 * nothrow version.
 *
 * @ingroup oz
 */
extern void operator delete[] ( void* ptr, const std::nothrow_t& ) throw();
