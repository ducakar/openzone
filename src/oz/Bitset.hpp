/*
 *  Bitset.hpp
 *
 *  Bitset
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

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
      explicit Bitset() : data( null ), size( 0 )
      {}

      /**
       * Copy construstor.
       * @param b the original Bitset
       */
      Bitset( const Bitset& b ) : data( new ulong[b.size] ), size( b.size )
      {
        aCopy( data, b.data, size );
      }

      /**
       * Allocate a new bitset that holds at least <code>nBits</code> bits. The size of
       * <code>data</code> array is adjusted to least multiplier of unit size that can hold the
       * requested number of bits.
       * @param nBits the number of bits the bitset should hold
       */
      explicit Bitset( int nBits )
      {
        size = ( nBits - 1 ) / ULONG_BITSIZE + 1;
        data = new ulong[size];
      }

      /**
       * Destructor.
       */
      ~Bitset()
      {
        if( data != null ) {
          delete[] data;
        }
      }

      /**
       * Copy operator.
       * @param b the original Bitset
       * @return copy
       */
      Bitset& operator = ( const Bitset& b )
      {
        assert( &b != this );

        if( size != b.size ) {
          setUnitSize( b.size );
        }
        aCopy( data, b.data, size );
        return *this;
      }

      /**
       * Equality operator.
       * @param b
       * @return true if all bits up to <code>length()</code> are equal.
       */
      bool operator == ( const Bitset& b )
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
      bool operator != ( const Bitset& b )
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
      operator const ulong* () const
      {
        return data;
      }

      /**
       * Get pointer to <code>data</code> array. Use with caution, since you can easily make buffer
       * overflows if you don't check the size of <code>data</code> array.
       * @return non-constant pointer to data array
       */
      operator ulong* ()
      {
        return data;
      }

      /**
       * Resize the <code>data</code> array. New size if specified in units.
       * @param nUnits
       */
      void setUnitSize( int nUnits )
      {
        soft_assert( size != nUnits );

        if( nUnits == size ) {
          return;
        }
        if( data != null ) {
          delete[] data;
        }
        size = nUnits;
        data = size == 0 ? null : new ulong[size];
      }

      /**
       * Resize the <code>data</code> array. New size if specified in bits.
       * @param nBits
       */
      void setSize( int nBits )
      {
        setUnitSize( nBits == 0 ? 0 : ( nBits - 1 ) / ULONG_BITSIZE + 1 );
      }

      /**
       * Size of bitset in bits.
       * @return number of bits the bitset can hold
       */
      int length() const
      {
        return size * ULONG_BITSIZE;
      }

      /**
       * Get i-th bit.
       * @param i bit index
       * @return bit
       */
      bool get( int i ) const
      {
        assert( 0 <= i && i < ( size * ULONG_BITSIZE ) );

        return ( data[i / ULONG_BITSIZE] & ( 1ul << ( i % ULONG_BITSIZE ) ) ) != 0ul;
      }

      /**
       * Set i-th bit to true.
       * @param i bit index
       */
      void set( int i )
      {
        assert( 0 <= i && i < ( size * ULONG_BITSIZE ) );

        data[i / ULONG_BITSIZE] |= 1ul << ( i % ULONG_BITSIZE );
      }

      /**
       * Set i-th bit to false.
       * @param i bit index
       */
      void clear( int i )
      {
        assert( 0 <= i && i < ( size * ULONG_BITSIZE ) );

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
        assert( 0 <= start && start <= end && end <= ( size * ULONG_BITSIZE ) );

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
        assert( 0 <= start && start <= end && end <= ( size * ULONG_BITSIZE ) );

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
       * Return AND of two bitsets. Bitsets must be the same size.
       * @param b the other bitset
       * @return AND of bitsets
       */
      Bitset operator & ( const Bitset& b ) const
      {
        assert( size == b.size );

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
        assert( size == b.size );

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
        assert( size == b.size );

        Bitset r( size );

        for( int i = 0; i < size; ++i ) {
          r.data[i] = data[i] ^ b.data[i];
        }
        return r;
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
        assert( size == b.size );

        Bitset r( size );

        for( int i = 0; i < size; ++i ) {
          if( ( data[i] & ~b.data[i] ) != 0ul ) {
            return false;
          }
        }
        return true;
      }

  };

}
