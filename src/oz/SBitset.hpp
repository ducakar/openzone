/*
 *  SBitset.hpp
 *
 *  SBitset with static storage
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "arrays.hpp"

namespace oz
{

  /**
   * SBitset data type.
   *
   * unit = ulong
   */
  template <int BITSIZE>
  class SBitset
  {
    static_assert( BITSIZE > 0, "SBitset size must be at least 1" );

    private:

      // Number of bits per unit.
      static const int ULONG_BITSIZE = sizeof( ulong ) * 8;
      static const int SIZE          = ( BITSIZE - 1 ) / ULONG_BITSIZE + 1;

      // Pointer to unit[] that holds the data.
      ulong data[SIZE];

    public:

      /**
       * Equality operator.
       * @param b
       * @return true if all bits up to <code>length()</code> are equal.
       */
      bool operator == ( const SBitset& b ) const
      {
        return aEquals( data, b.data, SIZE );
      }

      /**
       * Inequality operator.
       * @param b
       * @return false if all bits up to <code>length()</code> are equal.
       */
      bool operator != ( const SBitset& b ) const
      {
        return !aEquals( data, b.data, SIZE );
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
        return SIZE * ULONG_BITSIZE;
      }

      /**
       * Size of bitset in units.
       * @return
       */
      OZ_ALWAYS_INLINE
      int unitLength() const
      {
        return SIZE;
      }

      /**
       * Get i-th bit.
       * @param i bit index
       * @return bit
       */
      OZ_ALWAYS_INLINE
      bool get( int i ) const
      {
        hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

        return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
      }

      /**
       * Let say two bitsets are characteristic vectors of two sets. Return true if first set is a
       * subset of the second one. Other explanation: the result is true iff the following statement
       * is true: if first bitset has true on i-th position then the second bitset also has true on
       * i-th position.
       *
       * @param b the other bitset
       * @return implication of bitsets
       */
      bool isSubset( const SBitset& b ) const
      {
        SBitset r;

        for( int i = 0; i < SIZE; ++i ) {
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
        hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

        data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
      }

      /**
       * Set i-th bit to false.
       * @param i bit index
       */
      OZ_ALWAYS_INLINE
      void clear( int i )
      {
        hard_assert( uint( i ) < uint( SIZE * ULONG_BITSIZE ) );

        data[i / ULONG_BITSIZE] &= ~( 1ul << ( i % ULONG_BITSIZE ) );
      }

      /**
       * @return true, if all bits are true
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
       * @return true if all bits are false
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
       * @param start start index
       * @param end end index
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
       * @param start start index
       * @param end end index
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
       * Return bitset that has all bits inverted.
       * @return inverted bitset
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
       * @param b the other bitset
       * @return
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
       * @param b the other bitset
       * @return
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
       * @param b the other bitset
       * @return
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
       * @param b the other bitset
       * @return AND of bitsets
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
       * @param b the other bitset
       * @return OR of bitsets
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
       * @param b the other bitset
       * @return XOR of bitsets
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
