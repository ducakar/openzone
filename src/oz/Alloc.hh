/*
 * liboz - OpenZone core library.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file oz/Alloc.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Memory allocator.
 *
 * This class provides custom implementation for <tt>new</tt>/<tt>delete</tt> and
 * <tt>new[]</tt>/<tt>delete[]</tt> operators that provides memory allocation
 * statistics, specific alignment and optionally tracks memory leaks (if <tt>OZ_TRACK_LEAKS</tt>
 * config option is turned on).
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
    constexpr Alloc() = delete;

    /**
     * Enable/disable memory (de)allocation.
     *
     * If <tt>isLocked == true</tt>, any attempt of memory (de)allocation via
     * <tt>new</tt>/<tt>delete</tt> will abort the application with <code>System::abort()</code>.
     * if liboz was compiled without <tt>NDEBUG</tt> macro.
     *
     * Since <tt>isLocked</tt> is a weak symbol you can override it in some other module with a
     * custom definition
     * <code><pre>
     *   bool oz::Alloc::isLocked = false;
     * </pre></code>
     */
    static bool isLocked;

    /**
     * Align to the previous boundary.
     */
    OZ_ALWAYS_INLINE
    static constexpr size_t alignDown( size_t size )
    {
      return size & ~( ALIGNMENT - 1 );
    }

    /**
     * Align to the next boundary.
     */
    OZ_ALWAYS_INLINE
    static constexpr size_t alignUp( size_t size )
    {
      return ( ( size - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT;
    }

    /**
     * Align to the previous boundary.
     */
    template <typename Type>
    OZ_ALWAYS_INLINE
    static constexpr Type* alignDown( Type* p )
    {
      return reinterpret_cast<Type*>( size_t( p ) & ~( ALIGNMENT - 1 ) );
    }

    /**
     * Align to the next boundary.
     */
    template <typename Type>
    OZ_ALWAYS_INLINE
    static constexpr Type* alignUp( Type* p )
    {
      return reinterpret_cast<Type*>( ( size_t( p - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT );
    }

    /**
     * Print memory statistics into the global log.
     */
    static void printStatistics();

    /**
     * Print memory leaks into the global log.
     */
    static void printLeaks();

};

}
