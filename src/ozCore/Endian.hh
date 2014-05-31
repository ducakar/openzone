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
 * @file ozCore/Endian.hh
 *
 * `Endian` class.
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
#if OZ_BYTE_ORDER == 4321
    NATIVE = BIG
#else
    NATIVE = LITTLE
#endif
  };

  /**
   * Union for serialisation of `short` type.
   */
  union ShortToBytes
  {
    short value;   ///< `short` value.
    char  data[2]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `short` type.
   */
  union BytesToShort
  {
    char  data[2]; ///< Bytes.
    short value;   ///< `short` value.
  };

  /**
   * Union for serialisation of `ushort` type.
   */
  union UShortToBytes
  {
    ushort value;   ///< `ushort` value.
    char   data[2]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `ushort` type.
   */
  union BytesToUShort
  {
    char   data[2]; ///< Bytes.
    ushort value;   ///< `ushort` value.
  };

  /**
   * Union for serialisation of `int` type.
   */
  union IntToBytes
  {
    int  value;   ///< `int` value.
    char data[4]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `int` type.
   */
  union BytesToInt
  {
    char data[4]; ///< Bytes.
    int  value;   ///< `int` value.
  };

  /**
   * Union for serialisation of `uint` type.
   */
  union UIntToBytes
  {
    uint value;   ///< `uint` value.
    char data[4]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `uint` type.
   */
  union BytesToUInt
  {
    char data[4]; ///< Bytes.
    uint value;   ///< `uint` value.
  };

  /**
   * Union for serialisation of `long64` type.
   */
  union Long64ToBytes
  {
    long64 value;   ///< `long64` value.
    char   data[8]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `long64` type.
   */
  union BytesToLong64
  {
    char   data[8]; ///< Bytes.
    long64 value;   ///< `long64` value.
  };

  /**
   * Union for serialisation of `ulong64` type.
   */
  union ULong64ToBytes
  {
    ulong64 value;   ///< `ulong64` value.
    char    data[8]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `ulong64` type.
   */
  union BytesToULong64
  {
    char    data[8]; ///< Bytes.
    ulong64 value;   ///< `ulong64` value.
  };

  /**
   * Union for serialisation of `float` type.
   */
  union FloatToBytes
  {
    float value;   ///< `float` value.
    char  data[4]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `float` type.
   */
  union BytesToFloat
  {
    char  data[4]; ///< Bytes.
    float value;   ///< `float` value.
  };

  /**
   * Union for serialisation of `double` type.
   */
  union DoubleToBytes
  {
    double value;   ///< `double` value.
    char   data[8]; ///< Bytes.
  };

  /**
   * Union for deserialisation of `double` type.
   */
  union BytesToDouble
  {
    char   data[8]; ///< Bytes.
    double value;   ///< `double` value.
  };

public:

  /**
   * Forbid instances.
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
