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
 * @file oz/Endian.hh
 *
 * Endian class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * %Endian conversion.
 */
class Endian
{
  public:

    /**
     * Byte order.
     */
    enum Order
    {
      LITTLE = 0,
      BIG    = 1,
#ifdef OZ_BIG_ENDIAN
      NATIVE = BIG
#else
      NATIVE = LITTLE
#endif
    };

  public:

    /**
     * Forbid instances.
     */
    explicit Endian() = delete;

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
    static ushort bswap16( ushort s )
    {
      return ushort( s << 8 | s >> 8 );
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
    static uint bswap32( uint i )
    {
      return __builtin_bswap32( i );
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
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static ulong64 bswap64( ulong64 l )
    {
      return __builtin_bswap64( l );
    }

};

}
