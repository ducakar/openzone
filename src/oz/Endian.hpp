/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file oz/Endian.hpp
 */

#pragma once

#include "common.hpp"

namespace oz
{

/**
 * %Endian conversion.
 *
 * @ingroup oz
 */
class Endian
{
  private:

    /**
     * Singleton.
     */
    Endian();

  public:

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static short shuffle16( short s )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return s;
#else
      return short( ushort( s ) << 8 | ushort( s ) >> 8 );
#endif
    }

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static int shuffle32( int i )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return i;
#else
      return __builtin_bswap32( i );
#endif
    }

    /**
     * Swap byte order if OZ_BIG_ENDIAN_ARCH does not match OZ_BIG_ENDIAN_STREAM.
     */
    OZ_ALWAYS_INLINE
    static long64 shuffle64( long64 l )
    {
#if defined( OZ_BIG_ENDIAN_STREAM ) == defined( OZ_BIG_ENDIAN_ARCH )
      return l;
#else
      return __builtin_bswap64( l );
#endif
    }

};

}
