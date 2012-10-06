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
 * @file oz/Bitset.hh
 *
 * Bitset class.
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Packed array of bits with dynamically allocated storage.
 *
 * Bits are stored in an array of `ulong`s, so the its length in bits is always a multiple of
 * `sizeof( ulong ) * 8`.
 */
class Bitset
{
  private:

    /// Number of bits per unit.
    static const int ULONG_BITSIZE = sizeof( ulong ) * 8;

    ulong* data; ///< Pointer to array of units that holds the data.
    int    size; ///< Size of data array (in units, not in bits).

  public:

    /**
     * Allocate a new bitset that holds at least `nBits` bits.
     *
     * The size of `data` array is adjusted to the smallest unit number that can hold the requested
     * number of bits. No memory is allocated if `nBits == 0`.
     */
    explicit Bitset( int nBits = 0 )
    {
      if( nBits == 0 ) {
        data = nullptr;
        size = 0;
      }
      else {
        size = ( nBits - 1 ) / ULONG_BITSIZE + 1;
        data = new ulong[size];
      }
    }

    /**
     * Destructor.
     */
    ~Bitset()
    {
      delete[] data;
    }

    /**
     * Copy constructor, copies storage.
     */
    Bitset( const Bitset& b ) :
      data( b.size == 0 ? nullptr : new ulong[b.size] ), size( b.size )
    {
      aCopy<ulong>( data, b.data, b.size );
    }

    /**
     * Move constructor, moves storage.
     */
    Bitset( Bitset&& b ) :
      data( b.data ), size( b.size )
    {
      b.data = nullptr;
      b.size = 0;
    }

    /**
     * Copy operator, copies storage.
     *
     * Reuse existing storage only if it the size matches.
     */
    Bitset& operator = ( const Bitset& b )
    {
      if( &b == this ) {
        return *this;
      }

      if( size != b.size ) {
        delete[] data;

        data = b.size == 0 ? nullptr : new ulong[b.size];
        size = b.size;
      }

      aCopy<ulong>( data, b.data, b.size );

      return *this;
    }

    /**
     * Move operator, moves storage.
     */
    Bitset& operator = ( Bitset&& b )
    {
      if( &b == this ) {
        return *this;
      }

      delete[] data;

      data = b.data;
      size = b.size;

      b.data = nullptr;
      b.size = 0;

      return *this;
    }

    /**
     * True iff same size and respective bits are equal.
     */
    bool operator == ( const Bitset& b ) const
    {
      if( size != b.size ) {
        return false;
      }
      return aEquals<ulong>( data, b.data, size );
    }

    /**
     * True iff different size or any respective bits differ.
     */
    bool operator != ( const Bitset& b ) const
    {
      if( size != b.size ) {
        return true;
      }
      return !aEquals<ulong>( data, b.data, size );
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
      return size * ULONG_BITSIZE;
    }

    /**
     * Size in units.
     */
    OZ_ALWAYS_INLINE
    int unitLength() const
    {
      return size;
    }

    /**
     * True iff empty.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return size == 0;
    }

    /**
     * Get the `i`-th bit.
     */
    OZ_ALWAYS_INLINE
    bool get( int i ) const
    {
      hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

      return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
    }

    /**
     * %Set the `i`-th bit to true.
     */
    OZ_ALWAYS_INLINE
    void set( int i )
    {
      hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

      data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
    }

    /**
     * %Set the `i`-th bit to false.
     */
    OZ_ALWAYS_INLINE
    void clear( int i )
    {
      hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

      data[i / ULONG_BITSIZE] &= ~( 1ul << ( i % ULONG_BITSIZE ) );
    }

    /**
     * True iff all bits are true.
     */
    bool isAllSet() const
    {
      for( int i = 0; i < size; ++i ) {
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
      for( int i = 0; i < size; ++i ) {
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
     *
     * Both bitsets must be the same size.
     */
    bool isSubset( const Bitset& b ) const
    {
      hard_assert( size == b.size );

      for( int i = 0; i < size; ++i ) {
        if( ( data[i] & ~b.data[i] ) != 0ul ) {
          return false;
        }
      }
      return true;
    }

    /**
     * %Set bits from inclusively start to non-inclusively end to true.
     */
    void set( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * ULONG_BITSIZE ) );

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
     * %Set bits from inclusively start to non-inclusively end to false.
     */
    void clear( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * ULONG_BITSIZE ) );

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
     * %Set all bits to true.
     */
    void setAll()
    {
      aFill<ulong, ulong>( data, ~0ul, size );
    }

    /**
     * %Set all bits to false.
     */
    void clearAll()
    {
      aFill<ulong, ulong>( data, 0ul, size );
    }

    /**
     * NOT of the bitset.
     */
    Bitset operator ~ () const
    {
      Bitset r( size );

      for( int i = 0; i < size; ++i ) {
        r.data[i] = ~data[i];
      }
      return r;
    }

    /**
     * AND of two same-length bitsets.
     */
    Bitset& operator &= ( const Bitset& b )
    {
      for( int i = 0; i < size; ++i ) {
        data[i] &= b.data[i];
      }
      return *this;
    }

    /**
     * OR of two same-length bitsets.
     */
    Bitset& operator |= ( const Bitset& b )
    {
      for( int i = 0; i < size; ++i ) {
        data[i] |= b.data[i];
      }
      return *this;
    }

    /**
     * XOR of two same-length bitsets.
     */
    Bitset& operator ^= ( const Bitset& b )
    {
      for( int i = 0; i < size; ++i ) {
        data[i] ^= b.data[i];
      }
      return *this;
    }

    /**
     * Return AND of two same-length bitsets.
     */
    Bitset operator & ( const Bitset& b ) const
    {
      hard_assert( size == b.size );

      Bitset r( size );

      for( int i = 0; i < size; ++i ) {
        r.data[i] = data[i] & b.data[i];
      }
      return r;
    }

    /**
     * Return OR of two same-length bitsets.
     */
    Bitset operator | ( const Bitset& b ) const
    {
      hard_assert( size == b.size );

      Bitset r( size );

      for( int i = 0; i < size; ++i ) {
        r.data[i] = data[i] | b.data[i];
      }
      return r;
    }

    /**
     * Return XOR of two same-length bitsets.
     */
    Bitset operator ^ ( const Bitset& b ) const
    {
      hard_assert( size == b.size );

      Bitset r( size );

      for( int i = 0; i < size; ++i ) {
        r.data[i] = data[i] ^ b.data[i];
      }
      return r;
    }

    /**
     * For an empty bitset, allocate storage for `nBits` bits.
     */
    void allocate( int nBits )
    {
      hard_assert( size == 0 && nBits > 0 );

      int nUnits = ( nBits - 1 ) / ULONG_BITSIZE + 1;

      data = new ulong[nUnits];
      size = nUnits;
    }

    /**
     * Deallocate data.
     */
    void deallocate()
    {
      delete[] data;

      data = nullptr;
      size = 0;
    }

};

}
