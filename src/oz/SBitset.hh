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
 * @file oz/SBitset.hh
 *
 * SBitset class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Packed array of bits with static storage.
 *
 * Bits are stored in an array of `ulong`s, so the its length in bits is always a multiple of
 * `sizeof( ulong ) * 8`.
 */
template <int BITSIZE>
class SBitset
{
  static_assert( BITSIZE > 0, "SBitset size must be at least 1" );

  private:

    /// Number of bits per unit.
    static const int ULONG_BITSIZE = int( sizeof( ulong ) * 8 );

    /// Number of units.
    static const int SIZE = ( BITSIZE - 1 ) / ULONG_BITSIZE + 1;

    ulong data[SIZE]; ///< Pointer to array of units that holds the data.

  public:

    /**
     * True iff all bits are equal.
     */
    bool operator == ( const SBitset& b ) const
    {
      return aEquals<ulong>( data, b.data, SIZE );
    }

    /**
     * True any bit differ.
     */
    bool operator != ( const SBitset& b ) const
    {
      return !aEquals<ulong>( data, b.data, SIZE );
    }

    /**
     * Get constant pointer to `data` array.
     */
    OZ_ALWAYS_INLINE
    operator const ulong* () const
    {
      return data;
    }

    /**
     * Get pointer to `data` array.
     */
    OZ_ALWAYS_INLINE
    operator ulong* ()
    {
      return data;
    }

    /**
     * Size in bits.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return SIZE * ULONG_BITSIZE;
    }

    /**
     * Size in units.
     */
    OZ_ALWAYS_INLINE
    int unitLength() const
    {
      return SIZE;
    }

    /**
     * Always false since internal static array cannot have zero size.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return false;
    }

    /**
     * Get the i-th bit.
     */
    OZ_ALWAYS_INLINE
    bool get( int i ) const
    {
      hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

      return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
    }

    /**
     * Set the i-th bit to true.
     */
    OZ_ALWAYS_INLINE
    void set( int i )
    {
      hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

      data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
    }

    /**
     * Set the i-th bit to false.
     */
    OZ_ALWAYS_INLINE
    void clear( int i )
    {
      hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

      data[i / ULONG_BITSIZE] &= ~( 1ul << ( i % ULONG_BITSIZE ) );
    }

    /**
     * True iff all bits are true.
     */
    bool isAllSet() const
    {
      for( int i = 0; i < SIZE; ++i ) {
        if( data[i] != ~0ul ) {
          return false;
        }
      }
      return true;
    }

    /**
     * True iff all bits are false.
     */
    bool isAllClear() const
    {
      for( int i = 0; i < SIZE; ++i ) {
        if( data[i] != 0ul ) {
          return false;
        }
      }
      return true;
    }

    /**
     * True iff this bitset is a subset of the given bitset.
     *
     * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
     * subset of the second one. Other explanation: the result is true iff the following statement
     * is true: if first bitset has true on the i-th position then the second bitset also has true
     * on the i-th position.
     */
    bool isSubset( const SBitset& b ) const
    {
      for( int i = 0; i < SIZE; ++i ) {
        if( ( data[i] & ~b.data[i] ) != 0ul ) {
          return false;
        }
      }
      return true;
    }

    /**
     * Set bits from inclusively start to non-inclusively end to true.
     */
    void set( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( SIZE * ULONG_BITSIZE ) );

      int   startUnit   = start / ULONG_BITSIZE;
      int   startOffset = start % ULONG_BITSIZE;

      int   endUnit     = end / ULONG_BITSIZE;
      int   endOffset   = end % ULONG_BITSIZE;

      ulong startMask   = ~0ul << startOffset;
      ulong endMask     = ~( ~0ul << endOffset );

      if( startUnit == endUnit ) {
        data[startUnit] |= startMask & endMask;
      }
      else {
        data[startUnit] |= startMask;
        data[endUnit]   |= endMask;

        for( int i = startUnit + 1; i < endUnit; ++i ) {
          data[i] = ~0ul;
        }
      }
    }

    /**
     * Set bits from inclusively start to non-inclusively end to false.
     */
    void clear( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( SIZE * ULONG_BITSIZE ) );

      int   startUnit   = start / ULONG_BITSIZE;
      int   startOffset = start % ULONG_BITSIZE;

      int   endUnit     = end / ULONG_BITSIZE;
      int   endOffset   = end % ULONG_BITSIZE;

      ulong startMask   = ~( ~0ul << startOffset );
      ulong endMask     = ~0ul << endOffset;

      if( startUnit == endUnit ) {
        data[startUnit] &= startMask | endMask;
      }
      else {
        data[startUnit] &= startMask;
        data[endUnit]   &= endMask;

        for( int i = startUnit + 1; i < endUnit; ++i ) {
          data[i] = 0ul;
        }
      }
    }

    /**
     * Set all bits to true.
     */
    void setAll()
    {
      aSet<ulong, ulong>( data, ~0ul, SIZE );
    }

    /**
     * Set all bits to false.
     */
    void clearAll()
    {
      aSet<ulong, ulong>( data, 0ul, SIZE );
    }

    /**
     * NOT of the bitset.
     */
    SBitset operator ~ () const
    {
      SBitset r;

      for( int i = 0; i < SIZE; ++i ) {
        r.data[i] = ~data[i];
      }
      return r;
    }

    /**
     * AND of two bitsets.
     */
    SBitset& operator &= ( const SBitset& b )
    {
      for( int i = 0; i < SIZE; ++i ) {
        data[i] &= b.data[i];
      }
      return *this;
    }

    /**
     * OR of two bitsets.
     */
    SBitset& operator |= ( const SBitset& b )
    {
      for( int i = 0; i < SIZE; ++i ) {
        data[i] |= b.data[i];
      }
      return *this;
    }

    /**
     * XOR of two bitsets.
     */
    SBitset& operator ^= ( const SBitset& b )
    {
      for( int i = 0; i < SIZE; ++i ) {
        data[i] ^= b.data[i];
      }
      return *this;
    }

    /**
     * Return AND of two bitsets.
     */
    SBitset operator & ( const SBitset& b ) const
    {
      SBitset r;

      for( int i = 0; i < SIZE; ++i ) {
        r.data[i] = data[i] & b.data[i];
      }
      return r;
    }

    /**
     * Return OR of two bitsets.
     */
    SBitset operator | ( const SBitset& b ) const
    {
      SBitset r;

      for( int i = 0; i < SIZE; ++i ) {
        r.data[i] = data[i] | b.data[i];
      }
      return r;
    }

    /**
     * Return XOR of two bitsets.
     */
    SBitset operator ^ ( const SBitset& b ) const
    {
      SBitset r;

      for( int i = 0; i < SIZE; ++i ) {
        r.data[i] = data[i] ^ b.data[i];
      }
      return r;
    }

};

}
