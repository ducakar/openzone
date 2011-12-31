/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
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

    /**
     * Byte order.
     */
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
    constexpr Endian() = delete;

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static constexpr short bswap16( short s )
    {
      return short( ushort( s ) << 8 | ushort( s ) >> 8 );
    }

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static constexpr int bswap32( int i )
    {
      return int( __builtin_bswap32( uint( i ) ) );
    }

    /**
     * Swap byte order.
     */
    OZ_ALWAYS_INLINE
    static constexpr long64 bswap64( long64 l )
    {
      return long64( __builtin_bswap64( ulong64( l ) ) );
    }

};

}
