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
 * @file oz/Endian.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Endian conversion.
 *
 * @ingroup oz
 */
class Endian
{
  public:

    enum Order
    {
      LITTLE = 0,
      BIG    = 1,
#ifdef OZ_BIG_ENDIAN
      NATIVE = 1
#else
      NATIVE = 0
#endif
    };

    /**
     * Singleton.
     */
    Endian() = delete;

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static short bswap16( short s )
    {
      return short( ushort( s ) << 8 | ushort( s ) >> 8 );
    }

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static int bswap32( int i )
    {
      return int( __builtin_bswap32( uint( i ) ) );
    }

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static long64 bswap64( long64 l )
    {
      return long64( __builtin_bswap64( ulong64( l ) ) );
    }

    /**
     * Swap byte order if different from native.
     */
    OZ_ALWAYS_INLINE
    static short bswap16( short s, Order order )
    {
      return order == NATIVE ? s : short( ushort( s ) << 8 | ushort( s ) >> 8 );
    }

    /**
     * Swap byte order if different from native.
     */
    OZ_ALWAYS_INLINE
    static int bswap32( int i, Order order )
    {
      return order == NATIVE ? i : int( __builtin_bswap32( uint( i ) ) );
    }

    /**
     * Swap byte order if different from native.
     */
    OZ_ALWAYS_INLINE
    static long64 bswap64( long64 l, Order order )
    {
      return order == NATIVE ? l : long64( __builtin_bswap64( ulong64( l ) ) );
    }

};

}
