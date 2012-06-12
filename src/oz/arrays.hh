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
 * @file oz/arrays.hh
 *
 * Iterators and utility functions for arrays and array-like containers.
 */

#pragma once

#include "iterables.hh"

namespace oz
{

template <typename Elem>
class CIterator;

template <typename Elem>
CIterator<Elem> citer( const Elem* array, int count );

/**
 * %Array iterator with constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem>
class CIterator : public CIteratorBase<Elem>
{
  friend CIterator citer<Elem>( const Elem* array, int count );

  private:

    /// Base class type, convenience definition to make code cleaner.
    typedef CIteratorBase<Elem> B;

  protected:

    /// Successor of the last element, used to determine when the iterator becomes invalid.
    const Elem* past;

    /**
     * %Iterator for an array.
     *
     * @param start first array element.
     * @param past_ successor of the last element.
     */
    OZ_ALWAYS_INLINE
    explicit CIterator( const Elem* start, const Elem* past_ ) :
      B( start ), past( past_ )
    {}

  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    CIterator() :
      B( null ), past( null )
    {}

    /**
     * True while iterator has not passed all array elements.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return B::elem != past;
    }

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    CIterator& operator ++ ()
    {
      hard_assert( B::elem != past );

      ++B::elem;
      return *this;
    }

};

template <typename Elem>
class Iterator;

template <typename Elem>
Iterator<Elem> iter( Elem* array, int count );

/**
 * %Array iterator with non-constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem>
class Iterator : public IteratorBase<Elem>
{
  friend Iterator iter<Elem>( Elem* array, int count );

  private:

    /// Base class type, convenience definition to make code cleaner.
    typedef IteratorBase<Elem> B;

  protected:

    /// Successor of the last element, used to determine when the iterator becomes invalid.
    const Elem* past;

    /**
     * %Iterator for an array.
     *
     * @param start first array element.
     * @param past_ successor of the last element.
     */
    OZ_ALWAYS_INLINE
    explicit Iterator( Elem* start, const Elem* past_ ) :
      B( start ), past( past_ )
    {}

  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    Iterator() :
      B( null ), past( null )
    {}

    /**
     * True while iterator has not passed all array elements.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return B::elem != past;
    }

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    Iterator& operator ++ ()
    {
      hard_assert( B::elem != past );

      ++B::elem;
      return *this;
    }

};

/**
 * Create array iterator with constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer( const Elem* array, int count )
{
  return CIterator<Elem>( array, array + count );
}

/**
 * Create array iterator with non-constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter( Elem* array, int count )
{
  return Iterator<Elem>( array, array + count );
}

/**
 * Create static array iterator with constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem, size_t SIZE>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer( const Elem ( &array )[SIZE] )
{
  return citer<Elem>( array, SIZE );
}

/**
 * Create static array iterator with non-constant access to elements.
 *
 * @ingroup oz
 */
template <typename Elem, size_t SIZE>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter( Elem ( &array )[SIZE] )
{
  return iter<Elem>( array, SIZE );
}

/**
 * Copy array elements from the first to the last.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aCopy( Elem* aDest, const Elem* aSrc, int count )
{
  for( int i = 0; i < count; ++i ) {
    aDest[i] = aSrc[i];
  }
}

/**
 * Move array elements from the last to the first.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aReverseCopy( Elem* aDest, const Elem* aSrc, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    aDest[i] = aSrc[i];
  }
}

/**
 * Move array elements from the first to the last.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aMove( Elem* aDest, Elem* aSrc, int count )
{
  for( int i = 0; i < count; ++i ) {
    aDest[i] = static_cast<Elem&&>( aSrc[i] );
  }
}

/**
 * Move array elements from the last to the first.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aReverseMove( Elem* aDest, Elem* aSrc, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    aDest[i] = static_cast<Elem&&>( aSrc[i] );
  }
}

/**
 * Set array elements to the given value.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aSet( Elem* aDest, const Elem& value, int count )
{
  for( int i = 0; i < count; ++i ) {
    aDest[i] = value;
  }
}

/**
 * True iff respective elements are equal.
 *
 * @ingroup oz
 */
template <typename Elem>
inline bool aEquals( const Elem* aSrcA, const Elem* aSrcB, int count )
{
  int i = 0;
  while( i < count && aSrcA[i] == aSrcB[i] ) {
    ++i;
  }
  return i == count;
}

/**
 * True iff the given value is found in the array.
 *
 * @ingroup oz
 */
template <typename Elem>
inline bool aContains( const Elem* aSrc, const Elem& value, int count )
{
  int i = 0;
  while( i < count && !( aSrc[i] == value ) ) {
    ++i;
  }
  return i != count;
}

/**
 * Index of the first occurrence of the value or -1 if not found.
 *
 * @ingroup oz
 */
template <typename Elem>
inline int aIndex( const Elem* aSrc, const Elem& value, int count )
{
  int i = 0;
  while( i < count && !( aSrc[i] == value ) ) {
    ++i;
  }
  return i == count ? -1 : i;
}

/**
 * Index of the last occurrence of the value or -1 if not found.
 *
 * @ingroup oz
 */
template <typename Elem>
inline int aLastIndex( const Elem* aSrc, const Elem& value, int count )
{
  int i = count - 1;
  while( i >= 0 && !( aSrc[i] == value ) ) {
    --i;
  }
  return i;
}

/**
 * Delete objects referenced by elements and set all elements to <tt>null</tt>.
 *
 * If array elements are pointers to objects, delete all referenced objects and set all array
 * elements to <tt>null</tt>.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aFree( Elem* aDest, int count )
{
  for( int i = 0; i < count; ++i ) {
    delete aDest[i];
    aDest[i] = null;
  }
}

/**
 * Length of a static array.
 *
 * @ingroup oz
 */
template <typename Elem, int SIZE>
inline int aLength( const Elem ( & )[SIZE] )
{
  return SIZE;
}

/**
 * Reallocate array.
 *
 * Allocate new array of <tt>newCount</tt> elements, copy first <tt>count</tt> elements
 * of the source array <tt>aSrc</tt>to the newly created one and delete the source array.
 *
 * @return Newly allocated array.
 *
 * @ingroup oz
 */
template <typename Elem>
inline Elem* aRealloc( Elem* aSrc, int count, int newCount )
{
  Elem* aNew = new Elem[newCount];

  for( int i = 0; i < count; ++i ) {
    aNew[i] = static_cast<Elem&&>( aSrc[i] );
  }
  delete[] aSrc;

  return aNew;
}

/**
 * Insert an element at the given index.
 *
 * The remaining elements are shifted to make a gap. The last element is lost.
 *
 * @param aDest array.
 * @param value value of to be inserted.
 * @param index position where the element is to be inserted.
 * @param count number of elements in the array.
 *
 * @ingroup oz
 */
template <typename Elem, typename Elem_>
inline void aInsert( Elem* aDest, Elem_&& value, int index, int count )
{
  hard_assert( uint( index ) < uint( count ) );

  for( int i = count - 1; i > index; --i ) {
    aDest[i] = static_cast<Elem&&>( aDest[i - 1] );
  }
  aDest[index] = static_cast<Elem_&&>( value );
}

/**
 * Remove the element at the given index.
 *
 * The remaining elements are shifted to fill the gap.
 *
 * @param aDest array.
 * @param index position of the element to be removed.
 * @param count number of elements in the array.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aRemove( Elem* aDest, int index, int count )
{
  hard_assert( uint( index ) < uint( count ) );

  for( int i = index + 1; i < count; ++i ) {
    aDest[i - 1] = static_cast<Elem&&>( aDest[i] );
  }
}

/**
 * Reverse the order of array elements.
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aReverse( Elem* aDest, int count )
{
  int bottom = 0;
  int top = count - 1;

  while( bottom < top ) {
    swap<Elem>( aDest[bottom], aDest[top] );
    ++bottom;
    --top;
  }
}

/**
 * Utility function for aSort.
 *
 * <tt>Elem</tt> type must have <tt>operator \< ( const Elem\& )</tt> defined.
 * Quicksort algorithm is used which takes last element in a partition as a pivot so sorting a
 * sorted or nearly sorted array will take O(n^2) time instead of O(n log n) as in average case.
 * When a partition has at most 10 elements, selection sort is used.
 *
 * @param first pointer to first element in the array to be sorted.
 * @param last pointer to last element in the array.
 *
 * @ingroup oz
 */
template <typename Elem>
static void quicksort( Elem* first, Elem* last )
{
  // 8-14 seem as optimal thresholds for switching to selection sort.
  if( last - first > 10 ) {
    // Quicksort (last element is pivot).
    Elem* top    = first;
    Elem* bottom = last - 1;

    do {
      while( !( *last < *top ) && top <= bottom ) {
        ++top;
      }
      while( *last < *bottom && top < bottom ) {
        --bottom;
      }
      if( top >= bottom ) {
        break;
      }

      swap<Elem>( *top, *bottom );
    }
    while( true );

    if( top != last ) {
      swap<Elem>( *top, *last );
      quicksort<Elem>( top + 1, last );
    }
    quicksort<Elem>( first, top - 1 );
  }
  else {
    // Selection sort.
    for( Elem* i = first; i < last; ) {
      Elem* pivot = i;
      Elem* min   = i;
      ++i;

      for( Elem* j = i; j <= last; ++j ) {
        if( *j < *min ) {
          min = j;
        }
      }
      if( min != pivot ) {
        swap<Elem>( *min, *pivot );
      }
    }
  }
}

/**
 * Sort array (uses quicksort algorithm).
 *
 * @ingroup oz
 */
template <typename Elem>
inline void aSort( Elem* aSrc, int count )
{
  int last = count - 1;

  if( last > 0 ) {
    quicksort<Elem>( aSrc, &aSrc[last] );
  }
}

/**
 * Find an element using bisection.
 *
 * <tt>Elem</tt> type must have defined <tt>operator == ( const Key\&, const Elem\& )</tt> and
 * <tt>operator \< ( const Key\&, const Elem\& )</tt>.
 *
 * @param aSrc array.
 * @param key the key we are looking for.
 * @param count number of elements.
 * @return Index of the requested element or -1 if not found.
 *
 * @ingroup oz
 */
template <typename Elem, typename Key>
inline int aBisectFind( Elem* aSrc, const Key& key, int count )
{
  hard_assert( count >= 0 );

  if( count == 0 ) {
    return -1;
  }

  int a = 0;
  int b = count;

  // The algorithm ensures that ( a == 0 or data[a] <= key ) and ( b == count or key < data[b] ),
  // so the key may only lie on position a or nowhere.
  while( b - a > 1 ) {
    int c = ( a + b ) / 2;

    if( key < aSrc[c] ) {
      b = c;
    }
    else {
      a = c;
    }
  }
  return key == aSrc[a] ? a : -1;
}

/**
 * Find insert position for an element to be added using bisection.
 *
 * Returns an index such that
 * @code
 *   aSrc[index - 1] <= key && key < aSrc[index]
 * @endcode
 * If all elements are lesser, return <tt>count</tt> and if all elements are greater, return 0.
 * <tt>Elem</tt> type must have defined <tt>operator \< ( const Key\&, const Elem\& )</tt>.
 *
 * @param aSrc array.
 * @param key the key we are looking for.
 * @param count number of elements.
 * @return Index of least element greater than the key, or count if there's no such element.
 *
 * @ingroup oz
 */
template <typename Elem, typename Key>
inline int aBisectPosition( Elem* aSrc, const Key& key, int count )
{
  hard_assert( count >= 0 );

  int a = -1;
  int b = count;

  // The algorithm ensures that ( a == -1 or data[a] <= key ) and ( b == count or key < data[b] ),
  // so the key may only lie on position a or nowhere.
  while( b - a > 1 ) {
    int c = ( a + b ) / 2;

    if( key < aSrc[c] ) {
      b = c;
    }
    else {
      a = c;
    }
  }
  return a + 1;
}

}
