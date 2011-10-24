/*
 *  Alloc.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Alloc.hpp
 */

#include "common.hpp"

namespace oz
{

/**
 * Memory allocator.
 *
 * This class provides custom implementation for <tt>new</tt>/<tt>delete</tt> and
 * <tt>new[]</tt>/<tt>delete[]</tt> operators that provides memory allocation
 * statistics, specific alignment and optionally tracks memory leaks (if <tt>OZ_TRACE_LEAKS</tt>
 * config option is turned on).
 */
class Alloc
{
  public:

    /// Alignment of allocated storage returned by <tt>operator new</tt>.
#ifdef OZ_SIMD
    static const size_t ALIGNMENT = 16;
#else
    static const size_t ALIGNMENT = sizeof( size_t );
#endif

    static int    count;     ///< Current number of allocated memory chunks.
    static size_t amount;    ///< Amount of currently allocated memory.

    static int    sumCount;  ///< Number of all memory allocations.
    static size_t sumAmount; ///< Cumulative amount of all memory allocations.

    static int    maxCount;  ///< Top number to memory allocations.
    static size_t maxAmount; ///< Top amount of allocated memory.

  private:

    /**
     * Singleton.
     */
    Alloc();

  public:

    /**
     * Enable/disable memory (de)allocation.
     *
     * Any attempt of memory (de)allocation via <tt>new</tt>/<tt>delete</tt> will throw
     * a <code>std::bad_alloc</code> exception unless <tt>isLocked == true</tt>.<br/>
     * Since it is a weak symbol you can override it in some other module with a custom definition
     * <code><pre>
     *   bool oz::Alloc::isLocked = false;
     * </pre></code>
     */
    OZ_WEAK_SYMBOL
    static bool isLocked;

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
    template <typename Pointer>
    OZ_ALWAYS_INLINE
    static Pointer* alignDown( Pointer* p )
    {
      return reinterpret_cast<Pointer*>( size_t( p ) & ~( ALIGNMENT - 1 ) );
    }

    /**
     * Align to the next boundary.
     */
    template <typename Pointer>
    OZ_ALWAYS_INLINE
    static Pointer* alignUp( Pointer* p )
    {
      return reinterpret_cast<Pointer*>( ( size_t( p - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT );
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
