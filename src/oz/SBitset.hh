/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * Bits are stored in an array of <tt>ulong</tt>s, so the its length in bits is always a
 * multiple of <tt>sizeof( ulong ) * 8</tt>.
 *
 * @ingroup oz
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
      return aEquals( data, b.data, SIZE );
    }

    /**
     * True any bit differ.
     */
    bool operator != ( const SBitset& b ) const
    {
      return !aEquals( data, b.data, SIZE );
    }

    /**
     * Get constant pointer to <tt>data</tt> array.
     */
    OZ_ALWAYS_INLINE
    operator const ulong* () const
    {
      return data;
    }

    /**
     * Get pointer to <tt>data</tt> array.
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
     * Set bits from inclusively start to non-inclusively end to true.
     */
    void set( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( SIZE * ULONG_BITSIZE ) );

      int startUnit   = start / ULONG_BITSIZE;
      int startOffset = start % ULONG_BITSIZE;

      int endUnit     = end / ULONG_BITSIZE;
      int endOffset   = end % ULONG_BITSIZE;

      ulong startMask = ~0ul << startOffset;
      ulong endMask   = ~( ~0ul << endOffset );

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

      int startUnit   = start / ULONG_BITSIZE;
      int startOffset = start % ULONG_BITSIZE;

      int endUnit     = end / ULONG_BITSIZE;
      int endOffset   = end % ULONG_BITSIZE;

      ulong startMask = ~( ~0ul << startOffset );
      ulong endMask   = ~0ul << endOffset;

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
      aSet( data, ~0ul, SIZE );
    }

    /**
     * Set all bits to false.
     */
    void clearAll()
    {
      aSet( data, 0ul, SIZE );
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
