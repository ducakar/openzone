/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
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
 * statistics, specific alignment and optionally tracks memory leaks (if <tt>OZ_TRACE_LEAKS</tt>
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
    Alloc() = delete;

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
