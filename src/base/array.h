/*
 *  array.h
 *
 *  Array utility templates.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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
  template <typename Type>
  Iterator<Type> iterator( Type* array, const Type* past )
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
  template <typename Type>
  Iterator<Type> iterator( Type* array, int count )
  {
    return Iterator<Type>( array, array + count );
  }

  /**
   * Compare arrays (memcmp).
   * In contrast to memcmp it automagically calls != operator if comparing objects.
   * @param aSrcA pointer to the first element in the first array
   * @param aSrcB pointer to the first element in the second array
   * @param count number of elements to be compared
   * @return
   */
  template <typename Type>
  inline bool aEquals( const Type* aSrcA, const Type* aSrcB, int count )
  {
    for( int i = 0; i < count; ++i ) {
      if( aSrcA[i] != aSrcB[i] ) {
        return false;
      }
    }
    return true;
  }

  /**
   * Set array members to given value (memset).
   * In contrast with memset it calls constructor/destructor/assign operator on objects.
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
   * Copy array from first to last element (memcpy).
   * In contrast with memcpy it calls constructor/destructor/assign when copying objects.
   * On older GCCs it performs better than memopy on copying types bigger than one byte.
   * @param aDest pointer to the first element in the destination array
   * @param aSrc pointer to the first element in the source array
   * @param count number of elements to be copied
   */
  template <typename Type>
  inline void aCopy( Type* aDest, const Type* aSrc, int count )
  {
    assert( aDest != aSrc );

    for( int i = 0; i < count; ++i ) {
      Type t = aSrc[i];
      aDest[i] = t;
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
   * Find the first occurrence of an element.
   * @param aSrc pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurrence, -1 if not found
   */
  template <typename Type>
  inline int aIndex( const Type* aSrc, const Type& value, int count )
  {
    for( int i = 0; i < count; ++i ) {
      if( aSrc[i] == value ) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Find the last occurrence of an element.
   * @param aSrc pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurrence, -1 if not found
   */
  template <typename Type>
  inline int aLastIndex( const Type* aSrc, const Type& value, int count )
  {
    for( int i = count - 1; i <= 0; --i ) {
      if( aSrc[i] == value ) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Construct elements of an uninitialized array.
   * @param aDest
   * @param count
   */
  template <typename Type>
  inline void aConstruct( Type* aDest, int count )
  {
    for( int i = 0; i < count; ++i ) {
      new( &aDest[i] ) Type();
    }
  }

  /**
   * Construct elements via copy constructor from an already constructed array.
   * @param aDest
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
   * Destruct elements of an initialized array.
   * @param aDest
   * @param count
   */
  template <typename Type>
  inline void aDestruct( Type* aDest, int count )
  {
    for( int i = 0; i < count; ++i ) {
      aDest[i].~Type();
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
      if( aDest[i] != null ) {
        delete aDest[i];
        aDest[i] = null;
      }
    }
  }

  /**
   * Reallocate array (realloc).
   * Allocates new block of size newSize * typeof( Type ) and copies first "count" elements of
   * source array. newCount should be equal to or greater than count.
   * @param aDest pointer to the source array
   * @param count number of elements to be copied
   * @param newCount number of elements in the new array
   * @return
   */
  template <typename Type>
  inline Type* aRealloc( Type* aDest, int count, int newCount )
  {
    assert( count <= newCount );

    Type* aNew = new Type[newCount];

    for( int i = 0; i < count; ++i ) {
      aNew[i] = aDest[i];
    }
    delete[] aDest;

    return aNew;
  }

  /**
   * Utility function for aSort. It could also be called directly. Type must have operator& lt;
   * defined.
   * @param first pointer to first element in the array to be sorted
   * @param last pointer to last element in the array
   */
  template <typename Type>
  static void quicksort( Type* first, Type* last )
  {
    // 8-14 seem as optimal thresholds for switching to selection sort
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
   * time instead of O(n log n) as in general case. Type must have operator& lt; defined.
   * @param array pointer to the first element in the array
   * @param count number of elements to be sorted
   */
  template <typename Type>
  inline void aSort( Type* aSrc, int count )
  {
    quicksort( aSrc, aSrc + count - 1 );
  }

}
