/*
 * liboz - OpenZone core library.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file oz/Bitset.hh
 */

#pragma once

#include "arrays.hh"

namespace oz
{

/**
 * Packed array of bits with dynamically allocated storage.
 *
 * Bits are stored in an array of <tt>ulong</tt>s, so the its length in bits is always a
 * multiple of <tt>sizeof( ulong ) * 8</tt>.
 *
 * @ingroup oz
 */
class Bitset
{
  private:

    /// Number of bits per unit
    static const int ULONG_BITSIZE = sizeof( ulong ) * 8;

    ulong* data; ///< Pointer to array of units that holds the data.
    int    size; ///< Size of data array (in units, not in bits).

  public:

    /**
     * Create a new bitset without allocating any space.
     */
    Bitset() : data( null ), size( 0 )
    {}

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
    Bitset( const Bitset& b ) : data( b.size == 0 ? null : new ulong[b.size] ), size( b.size )
    {
      aCopy( data, b.data, b.size );
    }

    /**
     * Move constructor, moves storage.
     */
    Bitset( Bitset&& b ) : data( b.data ), size( b.size )
    {
      b.data = null;
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

        data = b.size == 0 ? null : new ulong[b.size];
        size = b.size;
      }

      aCopy( data, b.data, b.size );

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

      b.data = null;
      b.size = 0;

      return *this;
    }

    /**
     * Allocate a new bitset that holds at least <tt>nBits</tt> bits.
     *
     * The size of <tt>data</tt> array is adjusted to the smallest unit number that can hold
     * the requested number of bits.
     */
    explicit Bitset( int nBits )
    {
      if( nBits == 0 ) {
        data = null;
        size = 0;
      }
      else {
        size = ( nBits - 1 ) / ULONG_BITSIZE + 1;
        data = new ulong[size];
      }
    }

    /**
     * True iff same size and respective bits are equal.
     */
    bool operator == ( const Bitset& b ) const
    {
      if( size != b.size ) {
        return false;
      }
      return aEquals( data, b.data, size );
    }

    /**
     * True iff different size or any respective bits differ.
     */
    bool operator != ( const Bitset& b ) const
    {
      if( size != b.size ) {
        return true;
      }
      return !aEquals( data, b.data, size );
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
     * Get the i-th bit.
     */
    OZ_ALWAYS_INLINE
    bool get( int i ) const
    {
      hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

      return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
    }

    /**
     * True iff this bitset is a subset of the given bitset.
     *
     * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
     * subset of the second one. Other explanation: the result is true iff the following statement
     * is true: if first bitset has true on the i-th position then the second bitset also has true
     * on the i-th position.
     * Bitsets must be the same size.
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
     * Set the i-th bit to true.
     */
    OZ_ALWAYS_INLINE
    void set( int i )
    {
      hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

      data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
    }

    /**
     * Set the i-th bit to false.
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
     * Set bits from inclusively start to non-inclusively end to true.
     */
    void set( int start, int end )
    {
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * ULONG_BITSIZE ) );

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
      hard_assert( uint( start ) <= uint( end ) && uint( end ) <= uint( size * ULONG_BITSIZE ) );

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
      aSet( data, ~0ul, size );
    }

    /**
     * Set all bits to false.
     */
    void clearAll()
    {
      aSet( data, 0ul, size );
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
     * For an empty bitset, allocate storage for <tt>nBits</tt> bits.
     */
    void alloc( int nBits )
    {
      hard_assert( size == 0 && nBits > 0 );

      int nUnits = ( nBits - 1 ) / ULONG_BITSIZE + 1;

      data = new ulong[nUnits];
      size = nUnits;
    }

    /**
     * Deallocate data.
     */
    void dealloc()
    {
      delete[] data;

      data = null;
      size = 0;
    }

};

}
