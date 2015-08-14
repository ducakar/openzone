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
   * Union for serialisation of an integer value.
   */
  template <typename Type>
  union ToBytes
  {
    Type value;              ///< Value.
    char data[sizeof(Type)]; ///< The value represented as a sequence of bytes.
  };

  /**
   * Union for deserialisation of an integer value.
   */
  template <typename Type>
  union ToValue
  {
    char data[sizeof(Type)]; ///< Byte sequence.
    Type value;              ///< Value represented by the bytes.
  };

public:

  /**
   * Static class.
   */
  Endian() = delete;

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr short bswap(short s)
  {
    return short(s << 8 | ushort(s) >> 8);
  }

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr ushort bswap(ushort s)
  {
    return ushort(s << 8 | s >> 8);
  }

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr int bswap(int i)
  {
    return __builtin_bswap32(i);
  }

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr uint bswap(uint i)
  {
    return __builtin_bswap32(i);
  }

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr long64 bswap(long64 l)
  {
    return __builtin_bswap64(l);
  }

  /**
   * Swap byte order.
   */
  OZ_ALWAYS_INLINE
  static constexpr ulong64 bswap(ulong64 l)
  {
    return __builtin_bswap64(l);
  }

};

}
