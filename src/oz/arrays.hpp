/*
 *  arrays.hpp
 *
 *  Array utility templates.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "iterables.hpp"

namespace oz
{

  /**
   * Constant array iterator
   */
  template <typename Type>
  class CIterator : public CIteratorBase<Type>
  {
    private:

      typedef CIteratorBase<Type> B;

    protected:

      /**
       * Successor of the last element.
       * Is is used to determine when iterator becomes invalid.
       * It's not declared const so we can have use copy operator on Iterator.
       */
      const Type* past;

    public:

      /**
       * Default constructor returns an invalid iterator
       */
      explicit CIterator() : B( null ), past( null )
      {}

      /**
       * @param start first element for forward iterator or successor of last element for backward
       * iterator
       * @param past_ successor of last element for forward iterator or predecessor of first element
       * for backward iterator
       */
      explicit CIterator( const Type* start, const Type* past_ ) : B( start ), past( past_ )
      {}

      /**
       * Returns true while the iterator has not passed all the elements in the container and thus
       * points to a valid location.
       * @return
       */
      bool isValid() const
      {
        return B::elem != past;
      }

      /**
       * Advance to next element.
       * @return
       */
      CIterator& operator ++ ()
      {
        assert( B::elem != past );

        ++B::elem;
        return *this;
      }

  };

  /**
   * Array iterator
   */
  template <typename Type>
  class Iterator : public IteratorBase<Type>
  {
    private:

      typedef IteratorBase<Type> B;

    protected:

      /**
       * Successor of the last element.
       * Is is used to determine when iterator becomes invalid.
       * It's not declared const so we can have use copy operator on Iterator.
       */
      const Type* past;

    public:

      /**
       * Default constructor returns an invalid iterator
       */
      explicit Iterator() : B( null ), past( null )
      {}

      /**
       * @param start first element for forward iterator or successor of last element for backward
       * iterator
       * @param past_ successor of last element for forward iterator or predecessor of first element
       * for backward iterator
       */
      explicit Iterator( Type* start, const Type* past_ ) : B( start ), past( past_ )
      {}

      /**
       * Returns true while the iterator has not passed all the elements in the container and thus
       * points to a valid location.
       * @return
       */
      bool isValid() const
      {
        return B::elem != past;
      }

      /**
       * Advance to next element.
       * @return
       */
      Iterator& operator ++ ()
      {
        assert( B::elem != past );

        ++B::elem;
        return *this;
      }

  };

  /**
   * Make constant array iterator
   * Simplifies construction of array iterators.
   * Instead of
   * <code>CIterator&lt;int&gt;( array, array + 10 );</code>
   * you write
   * <code>citer( array, 10 );</code>
   * @param array
   * @param count
   * @return
   */
  template <typename Type>
  inline CIterator<Type> citer( const Type* array, int count )
  {
    return CIterator<Type>( array, array + count );
  }

  /**
   * Make array iterator
   * Simplifies construction of array iterators.
   * Instead of
   * <code>Iterator&lt;int&gt;( array, array + 10 );</code>
   * you write
   * <code>iter( array, 10 );</code>
   * @param array
   * @param count
   * @return
   */
  template <typename Type>
  inline Iterator<Type> iter( Type* array, int count )
  {
    return Iterator<Type>( array, array + count );
  }

  /**
   * Construct elements of an uninitialised array.
   * @param aDest
   * @param count
   */
  template <typename Type>
  inline void aConstruct( Type* aDest, int count )
  {
    for( int i = 0; i < count; ++i ) {
      new( &aDest[i] ) Type;
    }
  }

  /**
   * Construct elements via copy constructor from an already constructed array.
   * @param aDest
   * @param aSrc
   * @param count
   */
  template <typename Type>
  inline void aConstruct( Type* aDest, const Type* aSrc, int count )
  {
    for( int i = 0; i < count; ++i ) {
      new( &aDest[i] ) Type( aSrc[i] );
    }
  }

  /**
   * Destruct elements of an initialised array.
   * @param aDest
   * @param count
   */
  template <typename Type>
  inline void aDestruct( const Type* aSrc, int count )
  {
    for( int i = 0; i < count; ++i ) {
      aSrc[i].~Type();
    }
  }

  /**
   * Copy array from first to last element (memcpy).
   * In contrast with memcpy it calls copy operator on objects.
   * @param aDest pointer to the first element in the destination array
   * @param aSrc pointer to the first element in the source array
   * @param count number of elements to be copied
   */
  template <typename Type>
  inline void aCopy( Type* aDest, const Type* aSrc, int count )
  {
    assert( aDest != aSrc );

    for( int i = 0; i < count; ++i ) {
      aDest[i] = aSrc[i];
    }
  }

  /**
   * Copy array from last to first element.
   * It may be used where you cannot use aCopy due to source and destination overlapping.
   * @param aDest pointer to the first element in the destination array
   * @param aSrc pointer to the first element in the source array
   * @param count number of elements to be copied
   */
  template <typename Type>
  inline void aReverseCopy( Type* aDest, const Type* aSrc, int count )
  {
    assert( aDest != aSrc );

    for( int i = count - 1; i >= 0; --i ) {
      aDest[i] = aSrc[i];
    }
  }

  /**
   * Set array members to given value (memset).
   * In contrast with memset it calls copy operator on objects.
   * @param aDest pointer to the first element
   * @param value value to be set
   * @param count number of elements to be set
   */
  template <typename Type>
  inline void aSet( Type* aDest, const Type& value, int count )
  {
    for( int i = 0; i < count; ++i ) {
      aDest[i] = value;
    }
  }

  /**
   * Apply method on all array elements.
   * @param aDest
   * @param method
   * @param count
   */
  template <typename Type, typename Method>
  inline void aMap( Type* aDest, const Method& method, int count )
  {
    for( int i = 0; i < count; ++i ) {
      method( aDest[i] );
    }
  }

  /**
   * Compare arrays (memcmp).
   * In contrast to memcmp it calls != operator on objects.
   * @param aSrcA pointer to the first element in the first array
   * @param aSrcB pointer to the first element in the second array
   * @param count number of elements to be compared
   * @return
   */
  template <typename Type>
  inline bool aEquals( const Type* aSrcA, const Type* aSrcB, int count )
  {
    int i = 0;
    while( i < count && aSrcA[i] == aSrcB[i] ) {
      ++i;
    }
    return i == count;
  }

  /**
   * Return true if given value is found in the array.
   * @param aSrc
   * @param value
   * @param count
   * @return
   */
  template <typename Type>
  inline bool aContains( const Type* aSrc, const Type& value, int count )
  {
    int i = 0;
    while( i < count && !( aSrc[i] == value ) ) {
      ++i;
    }
    return i != count;
  }

  /**
   * Find the first occurrence of an element.
   * @param aSrc pointer to the first element in the array
   * @param value value we look for
   * @param count number of elements to be looked upon
   * @return index of the first occurrence, -1 if not found
   */
  template <typename Type>
  inline int aIndex( const Type* aSrc, const Type& value, int count )
  {
    int i = 0;
    while( i < count && !( aSrc[i] == value ) ) {
      ++i;
    }
    return i == count ? -1 : i;
  }

  /**
   * Find the last occurrence of an element.
   * @param aSrc pointer to the first element in the array
   * @param value value we look for
   * @param count number of elements to be looked upon
   * @return index of the first occurrence, -1 if not found
   */
  template <typename Type>
  inline int aLastIndex( const Type* aSrc, const Type& value, int count )
  {
    int i = count - 1;
    while( i >= 0 && !( aSrc[i] == value ) ) {
      --i;
    }
    return i;
  }

  /**
   * Remove element at the specified index. Shift (with move operator) the remaining elements to
   * fill the gap.
   * @param aDest pointer to the first element in the array
   * @param index position of the element to be removed
   * @param count number of elements in the array
   */
  template <typename Type>
  inline void aRemove( Type* aDest, int index, int count )
  {
    assert( 0 <= index && index < count );

    for( int i = index + 1; i < count; ++i ) {
      aDest[i - 1] = aDest[i];
    }
  }

  /**
   * Call delete on each non-null element of an array of pointers and set all elements to null.
   * @param aDest pointer to the first element in the array
   * @param count number of elements
   */
  template <typename Type>
  inline void aFree( Type* aDest, int count )
  {
    for( int i = 0; i < count; ++i ) {
      if( !( aDest[i] == null ) ) {
        delete aDest[i];
        aDest[i] = null;
      }
    }
  }

  /**
   * Length of a static array.
   * This function does not work with pointers to array.
   * @param array
   * @return
   */
  template <typename Type>
  inline int aLength( const Type& aSrc )
  {
    return sizeof( aSrc ) / sizeof( aSrc[0] );
  }

  /**
   * Reallocate array (realloc).
   * Allocates new block of size newSize * sizeof( Type ) and copies first "count" elements of
   * source array. newCount should be equal to or greater than count.
   * @param aDest pointer to the source array
   * @param count number of elements to be copied
   * @param newCount number of elements in the new array
   * @return
   */
  template <typename Type>
  inline Type* aRealloc( Type* aDest, int count, int newCount )
  {
    Type* aNew = new Type[newCount];

    for( int i = 0; i < count; ++i ) {
      aNew[i] = aDest[i];
    }
    delete[] aDest;

    return aNew;
  }

  /**
   * Utility function for aSort. It could also be called directly.
   * Type must have operator &lt; defined.
   * @param first pointer to first element in the array to be sorted
   * @param last pointer to last element in the array
   */
  template <typename Type>
  static void quicksort( Type* first, Type* last )
  {
    // 8-14 seem as an optimal thresholds for switching to selection sort
    if( last - first > 8 ) {
      // quicksort
      Type* top = first;
      Type* bottom = last - 1;

      do {
        while( top <= bottom && !( *last < *top ) ) {
          ++top;
        }
        while( top < bottom && *last < *bottom ) {
          --bottom;
        }
        if( top >= bottom ) {
          break;
        }
        swap( *top, *bottom );
      }
      while( true );

      swap( *top, *last );

      quicksort( first, top - 1 );
      quicksort( top + 1, last );
    }
    else {
      // selection sort
      for( Type* i = first; i < last; ) {
        Type* pivot = i;
        Type* min = i;
        ++i;

        for( Type* j = i; j <= last; ++j ) {
          if( *j < *min ) {
            min = j;
          }
        }
        swap( *pivot, *min );
      }
    }
  }

  /**
   * Perform quicksort on the array. Recursive quicksort algorithm is used which takes first
   * element in partition as a pivot so sorting a sorted or nearly sorted array will take O(n^2)
   * time instead of O(n log n) as in general case.
   * Type must have operator &lt; defined.
   * @param array pointer to the first element in the array
   * @param count number of elements to be sorted
   */
  template <typename Type>
  inline void aSort( Type* aSrc, int count )
  {
    quicksort( aSrc, aSrc + count - 1 );
  }

  /**
   * Find an element using bisection.
   * Type must have operators == and &lt; defined.
   * @param aSrc non-empty array
   * @param key the key we are looking for
   * @param count
   * @return index of requested element or -1 if not found
   */
  template <typename Type, typename Key>
  inline int aBisectFind( Type* aSrc, const Key& key, int count )
  {
    if( count == 0 ) {
      return -1;
    }

    int a = 0;
    int b = count;

    // we must check this or the next assumption may not hold
    if( key < aSrc[0] ) {
      return -1;
    }

    // Note that the algorithm ensures data[a] <= key and key < data[b] all the time, so the
    // key may only lie on position a.
    do {
      int c = ( a + b ) / 2;

      if( key < aSrc[c] ) {
        b = c;
      }
      else {
        a = c;
      }
    }
    while( b - a > 1 );

    return key == aSrc[a] ? a : -1;
  }

  /**
   * Find insert position for an element to be added using bisection.
   * Returns an index such that aSrc[index - 1] <= key && key < aSrc[index]. If all elements are
   * lesser, return count, if all elements are greater, return 0.
   * Type must have operators == and &lt; defined.
   * @param aSrc
   * @param key the key we are looking for
   * @param count
   * @return index of least element greater than the key, or count if there's no such element
   */
  template <typename Type, typename Key>
  inline int aBisectPosition( Type* aSrc, const Key& key, int count )
  {
    if( count == 0 ) {
      return 0;
    }

    int a = 0;
    int b = count;

    // we must check this or the next assumption may not hold
    if( key < aSrc[0] ) {
      return 0;
    }

    // Note that the algorithm ensures data[a] <= key and key < data[b] all the time, so the
    // key may only lie on position a.
    do {
      int c = ( a + b ) / 2;

      if( key < aSrc[c] ) {
        b = c;
      }
      else {
        a = c;
      }
    }
    while( b - a > 1 );

    return a + 1;
  }

}
