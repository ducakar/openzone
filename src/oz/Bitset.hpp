/*
 *  Bitset.hpp
 *
 *  Bitset
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

  /**
   * Bitset data type.
   *
   * unit = ulong
   */
  class Bitset
  {
    private:

      // Number of bits per unit.
      static const int ULONG_BITSIZE = sizeof( ulong ) * 8;

      // Pointer to unit[] that holds the data.
      ulong* data;

      // Size of data array (in units, not in bits).
      int    size;

    public:

      /**
       * Create a new bitset without allocating any space.
       */
      Bitset() : data( null ), size( 0 )
      {}

      /**
       * Copy constructor.
       * @param b the original Bitset
       */
      Bitset( const Bitset& b ) : data( b.size == 0 ? null : new ulong[b.size] ), size( b.size )
      {
        aCopy( data, b.data, b.size );
      }

      /**
       * Copy operator.
       * @param b the original Bitset
       * @return
       */
      Bitset& operator = ( const Bitset& b )
      {
        hard_assert( &b != this );

        if( size != b.size ) {
          delete[] data;
          data = new ulong[b.size];
          size = b.size;
        }

        aCopy( data, b.data, b.size );
        return *this;
      }

      /**
       * Destructor.
       */
      ~Bitset()
      {
        delete[] data;
      }

      /**
       * Allocate a new bitset that holds at least <code>nBits</code> bits. The size of
       * <code>data</code> array is adjusted to least multiplier of unit size that can hold the
       * requested number of bits.
       * @param nBits the number of bits the bitset should hold
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
       * Equality operator.
       * @param b
       * @return true if all bits up to <code>length()</code> are equal.
       */
      bool operator == ( const Bitset& b ) const
      {
        if( size != b.size ) {
          return false;
        }
        return aEquals( data, b.data, size );
      }

      /**
       * Inequality operator.
       * @param b
       * @return false if all bits up to <code>length()</code> are equal.
       */
      bool operator != ( const Bitset& b ) const
      {
        if( size != b.size ) {
          return true;
        }
        return !aEquals( data, b.data, size );
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      OZ_ALWAYS_INLINE
      operator const ulong* () const
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      OZ_ALWAYS_INLINE
      operator ulong* ()
      {
        return data;
      }

      /**
       * Size of bitset in bits.
       * @return number of bits the bitset can hold
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return size * ULONG_BITSIZE;
      }

      /**
       * Size of bitset in units.
       * @return
       */
      OZ_ALWAYS_INLINE
      int unitLength() const
      {
        return size;
      }

      /**
       * Get i-th bit.
       * @param i bit index
       * @return bit
       */
      OZ_ALWAYS_INLINE
      bool get( int i ) const
      {
        hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

        return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
      }

      /**
       * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
       * subset of the second one. Other explanation: the result is true iff the following statement
       * is true: if first bitset has true on i-th position then the second bitset also has true on
       * i-th position.
       * Bitsets must be the same size.
       *
       * @param b the other bitset
       * @return implication of bitsets
       */
      bool isSubset( const Bitset& b ) const
      {
        hard_assert( size == b.size );

        Bitset r( size );

        for( int i = 0; i < size; ++i ) {
          if( ( data[i] & ~b.data[i] ) != 0ul ) {
            return false;
          }
        }
        return true;
      }

      /**
       * Set i-th bit to true.
       * @param i bit index
       */
      OZ_ALWAYS_INLINE
      void set( int i )
      {
        hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

        data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
      }

      /**
       * Set i-th bit to false.
       * @param i bit index
       */
      OZ_ALWAYS_INLINE
      void clear( int i )
      {
        hard_assert( uint( i ) < uint( size * ULONG_BITSIZE ) );

        data[i / ULONG_BITSIZE] &= ~( 1ul << ( i % ULONG_BITSIZE ) );
      }

      /**
       * @return true, if all bits are true
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
       * @return true if all bits are false
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
       * @param start start index
       * @param end end index
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
       * @param start start index
       * @param end end index
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
       * Return bitset that has all bits inverted.
       * @return inverted bitset
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
       * AND of two bitsets.
       * @param b the other bitset
       * @return
       */
      Bitset& operator &= ( const Bitset& b )
      {
        for( int i = 0; i < size; ++i ) {
          data[i] &= b.data[i];
        }
        return *this;
      }

      /**
       * OR of two bitsets.
       * @param b the other bitset
       * @return
       */
      Bitset& operator |= ( const Bitset& b )
      {
        for( int i = 0; i < size; ++i ) {
          data[i] |= b.data[i];
        }
        return *this;
      }

      /**
       * XOR of two bitsets.
       * @param b the other bitset
       * @return
       */
      Bitset& operator ^= ( const Bitset& b )
      {
        for( int i = 0; i < size; ++i ) {
          data[i] ^= b.data[i];
        }
        return *this;
      }

      /**
       * Return AND of two bitsets. Bitsets must be the same size.
       * @param b the other bitset
       * @return AND of bitsets
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
       * Return OR of two bitsets. Bitsets must be the same size.
       * @param b the other bitset
       * @return OR of bitsets
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
       * Return XOR of two bitsets. Bitsets must be the same size.
       * @param b the other bitset
       * @return XOR of bitsets
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
       * Allocates capacity for <code>nBits</code>.
       * @param nUnits
       */
      void alloc( int nBits )
      {
        hard_assert( size == 0 && nBits > 0 );

        int nUnits = nBits == 0 ? 0 : ( nBits - 1 ) / ULONG_BITSIZE + 1;

        size = nUnits;
        data = size == 0 ? null : new ulong[size];
      }

      /**
       * Deallocate resources.
       */
      void dealloc()
      {
        delete[] data;

        data = null;
        size = 0;
      }

  };

}
