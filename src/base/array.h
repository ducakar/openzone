/*
 *  array.h
 *
 *  Array utility templates.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  /**
   * Array iterator.
   * Somehow simplifies construction of array iterators.
   * Instead of
   * <code>Iterator&lt;int&gt;( array, array + 10 );</code>
   * you write
   * <code>iterator( array, array + 10 );</code>
   * @param array
   * @param count
   * @return
   */
  template <class Type>
  Iterator<Type> iterator( Type *array, const Type *past )
  {
    return Iterator<Type>( array, past );
  }

  /**
   * Array iterator.
   * Somehow simplifies construction of array iterators.
   * Instead of
   * <code>Iterator&lt;int&gt;( array, array + 10 );</code>
   * you write
   * <code>iterator( array, 10 );</code>
   * @param array
   * @param count
   * @return
   */
  template <class Type>
  Iterator<Type> iterator( Type *array, int count )
  {
    return Iterator<Type>( array, array + count );
  }

  /**
   * Set array members to given value (memset).
   * In contrast with memset it calls constructor/destructor/assign operator on objects.
   * @param dest pointer to the first element
   * @param value value to be set
   * @param count number of elements to be set
   */
  template <class Type>
  inline void aSet( Type *array, const Type &value, int count )
  {
    for( int i = 0; i < count; i++ ) {
      array[i] = value;
    }
  }

  /**
   * Compare arrays (memcmp).
   * In contrast to memcmp it automagically calls != operator if comparing objects.
   * @param srcA pointer to the first element in the first array
   * @param srcB pointer to the first element in the second array
   * @param count number of elements to be compared
   * @return
   */
  template <class Type>
  inline bool aEquals( const Type *srcA, const Type *srcB, int count )
  {
    for( int i = 0; i < count; i++ ) {
      if( srcA[i] != srcB[i] ) {
        return false;
      }
    }
    return true;
  }

  /**
   * Copy array from first to last element (memcpy).
   * In contrast with memcpy it calls constructor/destructor/assign when copying objects.
   * On older GCCs it performs better than memcopy on copying types bigger than one byte.
   * @param dest pointer to the first element in the destination array
   * @param src pointer to the first element in the source array
   * @param count number of elements to be copied
   */
  template <class Type>
  inline void aCopy( Type *dest, const Type *src, int count )
  {
    assert( dest != src );

    for( int i = 0; i < count; i++ ) {
      Type t = src[i];
      dest[i] = t;
    }
  }

  /**
   * Copy array from last to first element.
   * It may be used where you cannot use aCopy due to source and destination overlapping.
   * @param dest pointer to the first element in the destination array
   * @param src pointer to the first element in the source array
   * @param count number of elements to be copied
   */
  template <class Type>
  inline void aReverseCopy( Type *dest, const Type *src, int count )
  {
    assert( dest != src );

    for( int i = count - 1; i >= 0; i-- ) {
      dest[i] = src[i];
    }
  }

  /**
   * Find the first occurrence of an element.
   * @param array pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurrence, -1 if not found
   */
  template <class Type>
  inline int aIndex( const Type *array, const Type &value, int count )
  {
    for( int i = 0; i < count; i++ ) {
      if( array[i] == value ) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Find the last occurrence of an element.
   * @param array pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurrence, -1 if not found
   */
  template <class Type>
  inline int aLastIndex( const Type *array, const Type &value, int count )
  {
    for( int i = count - 1; i <= 0; i-- ) {
      if( array[i] == value ) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Call delete on array elements (that have been previously allocated with the new call).
   * @param array pointer to the first element in the array
   * @param count number of elements
   */
  template <class Type>
  inline void aFree( const Type *array, int count )
  {
    for( int i = 0; i < count; i++ ) {
      delete array[i];
    }
  }

  /**
   * Reallocate array (realloc).
   * Allocates new block of size newSize * typeof( Type ) and copies first "count" elements of
   * source array. newCount should be equal to or greater than count.
   * @param array pointer to the source array
   * @param count number of elements to be copied
   * @param newCount number of elements in the new array
   * @return
   */
  template <class Type>
  inline Type *aRealloc( Type *array, int count, int newCount )
  {
    assert( count <= newCount );

    Type *newArray = new Type[newCount];

    aCopy( newArray, array, count );
    delete[] array;

    return newArray;
  }

  /**
   * Utility function for aSort. It could also be called directly. Type must have operator &lt;
   * defined.
   * @param first pointer to first element in the array to be sorted
   * @param last pointer to last element in the array
   */
  template <class Type>
  void quicksort( Type *first, Type *last )
  {
    // 8-14 seem as optimal thresholds for switching to selection sort
    if( last - first > 10 ) {
      // quicksort
      Type *top = first;
      Type *bottom = last - 1;

      do {
        while( top <= bottom && !( *last < *top ) ) {
          top++;
        }
        while( top < bottom && *last < *bottom ) {
          bottom--;
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
      for( Type *i = first; i < last; ) {
        Type *pivot = i;
        Type *min = i;
        i++;

        for( Type *j = i; j <= last; j++ ) {
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
   * time instead of O(n log n) as in general case. Type must have operator &lt; defined.
   * @param array pointer to the first element in the array
   * @param count number of elements to be sorted
   */
  template <class Type>
  inline void aSort( Type *array, int count )
  {
    quicksort( array, array + count - 1 );
  }

}
