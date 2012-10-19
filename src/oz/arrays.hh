/*
 * liboz - OpenZone Core Library.
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

/**
 * %Array iterator.
 *
 * It should not be used directly but appropriate iterator types should be typedef'd to it.
 */
template <typename Elem>
class ArrayIterator : public IteratorBase<Elem>
{
  protected:

    using IteratorBase<Elem>::elem;

    /// Successor of the last element, used to determine when the iterator becomes invalid.
    const Elem* past;

  public:

    /**
     * Default constructor, creates an invalid iterator.
     */
    OZ_ALWAYS_INLINE
    explicit ArrayIterator() :
      IteratorBase<Elem>( nullptr ), past( nullptr )
    {}

    /**
     * %Array iterator.
     *
     * @param first first array element.
     * @param past_ successor of the last element.
     */
    OZ_ALWAYS_INLINE
    explicit ArrayIterator( Elem* first, Elem* past_ ) :
      IteratorBase<Elem>( first ), past( past_ )
    {}

    /**
     * True as long as iterator has not passed all array elements.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return elem != past;
    }

    /**
     * Advance to the next element.
     */
    OZ_ALWAYS_INLINE
    ArrayIterator& operator ++ ()
    {
      hard_assert( elem != past );

      ++elem;
      return *this;
    }

};

#if defined( OZ_ECLIPSE ) || ( defined( OZ_GCC ) && OZ_GCC < 407 )

template <typename Elem>
class CIterator : public ArrayIterator<const Elem>
{
  public:

    OZ_ALWAYS_INLINE
    explicit CIterator() = default;

    OZ_ALWAYS_INLINE
    explicit CIterator( const Elem* first, const Elem* past ) :
      ArrayIterator<const Elem>( first, past )
    {}

};

template <typename Elem>
class Iterator : public ArrayIterator<Elem>
{
  public:

    OZ_ALWAYS_INLINE
    explicit Iterator() = default;

    OZ_ALWAYS_INLINE
    explicit Iterator( Elem* first, Elem* past ) :
      ArrayIterator<Elem>( first, past )
    {}

};

#else

/**
 * %Array iterator with constant access to elements.
 */
template <typename Elem>
using CIterator = ArrayIterator<const Elem>;

/**
 * %Array iterator with non-constant access to elements.
 */
template <typename Elem>
using Iterator = ArrayIterator<Elem>;

#endif

/**
 * Create array iterator with constant element access.
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer( const Elem* array, int count )
{
  return CIterator<Elem>( array, array + count );
}

/**
 * Create array iterator with non-constant element access.
 */
template <typename Elem>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter( Elem* array, int count )
{
  return Iterator<Elem>( array, array + count );
}

/**
 * Create static array iterator with element constant access.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline CIterator<Elem> citer( const Elem ( & array )[COUNT] )
{
  return CIterator<Elem>( array, array + COUNT );
}

/**
 * Create static array iterator with non-constant element access.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline Iterator<Elem> iter( Elem ( & array )[COUNT] )
{
  return Iterator<Elem>( array, array + COUNT );
}

/**
 * Length of a static array.
 */
template <typename Elem, int COUNT>
OZ_ALWAYS_INLINE
inline int aLength( const Elem ( & )[COUNT] )
{
  return COUNT;
}

/**
 * Copy array elements from the first to the last.
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
 */
template <typename Elem>
inline void aCopyBackward( Elem* aDest, const Elem* aSrc, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    aDest[i] = aSrc[i];
  }
}

/**
 * Move array elements from the first to the last.
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
 */
template <typename Elem>
inline void aMoveBackward( Elem* aDest, Elem* aSrc, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    aDest[i] = static_cast<Elem&&>( aSrc[i] );
  }
}

/**
 * %Set array elements to the given value.
 */
template <typename Elem, typename Value = Elem>
inline void aFill( Elem* aDest, const Value& value, int count )
{
  for( int i = 0; i < count; ++i ) {
    aDest[i] = value;
  }
}

/**
 * Swap array elements.
 */
template <typename Elem>
inline void aSwap( Elem* aDestA, Elem* aDestB, int count )
{
  for( int i = 0; i < count; ++i ) {
    Elem t = static_cast<Elem&&>( aDestA[i] );
    aDestA[i] = static_cast<Elem&&>( aDestB[i] );
    aDestB[i] = static_cast<Elem&&>( t );
  }
}

/**
 * Swap elements of two same-size static arrays.
 */
template <typename Elem, int COUNT>
inline void aSwap( Elem ( & aDestA )[COUNT], Elem ( & aDestB )[COUNT] )
{
  for( int i = 0; i < COUNT; ++i ) {
    Elem t = static_cast<Elem&&>( aDestA[i] );
    aDestA[i] = static_cast<Elem&&>( aDestB[i] );
    aDestB[i] = static_cast<Elem&&>( t );
  }
}

/**
 * True iff respective elements are equal.
 */
template <typename Elem>
inline bool aEquals( const Elem* aSrcA, const Elem* aSrcB, int count )
{
  for( int i = 0; i < count; ++i ) {
    if( !( aSrcA[i] == aSrcB[i] ) ) {
      return false;
    }
  }
  return true;
}

/**
 * True iff the given value is found in the array.
 */
template <typename Elem, typename Value = Elem>
inline bool aContains( const Elem* aSrc, const Value& value, int count )
{
  for( int i = 0; i < count; ++i ) {
    if( aSrc[i] == value ) {
      return true;
    }
  }
  return false;
}

/**
 * Pointer to the first occurrence or `nullptr` if not found.
 */
template <typename Elem, typename Value = Elem>
inline Elem* aFind( Elem* aSrc, const Value& value, int count )
{
  for( int i = 0; i < count; ++i ) {
    if( aSrc[i] == value ) {
      return &aSrc[i];
    }
  }
  return nullptr;
}

/**
 * Pointer to the last occurrence or `nullptr` if not found.
 */
template <typename Elem, typename Value = Elem>
inline Elem* aFindLast( Elem* aSrc, const Value& value, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    if( aSrc[i] == value ) {
      return &aSrc[i];
    }
  }
  return nullptr;
}

/**
 * Index of the first occurrence of the value or -1 if not found.
 */
template <typename Elem, typename Value = Elem>
inline int aIndex( const Elem* aSrc, const Value& value, int count )
{
  for( int i = 0; i < count; ++i ) {
    if( aSrc[i] == value ) {
      return i;
    }
  }
  return -1;
}

/**
 * Index of the last occurrence of the value or -1 if not found.
 */
template <typename Elem, typename Value = Elem>
inline int aLastIndex( const Elem* aSrc, const Value& value, int count )
{
  for( int i = count - 1; i >= 0; --i ) {
    if( aSrc[i] == value ) {
      return i;
    }
  }
  return -1;
}

/**
 * Delete objects referenced by elements and set all elements to `nullptr`.
 */
template <typename Elem>
inline void aFree( Elem* aDest, int count )
{
  for( int i = 0; i < count; ++i ) {
    delete aDest[i];
    aDest[i] = nullptr;
  }
}

/**
 * Reallocate array.
 *
 * Allocate new array of `newCount` elements, copy first `count` elements of the source array
 * `aSrc`to the newly created one and delete the source array.
 *
 * @return Newly allocated array.
 */
template <typename Elem>
inline Elem* aReallocate( Elem* aSrc, int count, int newCount )
{
  Elem* aNew = nullptr;

  if( newCount != 0 ) {
    aNew = new Elem[newCount];

    for( int i = 0; i < count; ++i ) {
      aNew[i] = static_cast<Elem&&>( aSrc[i] );
    }
  }
  delete[] aSrc;

  return aNew;
}

/**
 * Reverse the order of array elements.
 */
template <typename Elem>
inline void aReverse( Elem* aDest, int count )
{
  int bottom = 0;
  int top    = count - 1;

  while( bottom < top ) {
    swap<Elem>( aDest[bottom], aDest[top] );
    ++bottom;
    --top;
  }
}

/**
 * Utility function for aSort.
 *
 * `Elem` type must have `operator < ( const Elem& )` defined.
 * Quicksort algorithm is used which takes last element in a partition as a pivot so sorting a
 * sorted or nearly sorted array will take O(n^2) time instead of O(n log n) as in average case.
 * When a partition has at most 10 elements, selection sort is used.
 *
 * @param first pointer to first element in the array to be sorted.
 * @param last pointer to last element in the array.
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
 * Find index in a sorted array such that `array[index] <= key && key < array[index + 1]`.
 *
 * If all elements are lesser return `count - 1` and if all elements are greater return -1.
 * `Elem` type must have defined `bool operator < ( const Key&, const Elem& ) const`.
 *
 * @param aSrc array.
 * @param key the key we are looking for.
 * @param count number of elements.
 * @return Index of the last element not greater than `key`, -1 otherwise.
 */
template <typename Elem, typename Key = Elem>
inline int aBisection( Elem* aSrc, const Key& key, int count )
{
  int a = -1;
  int b = count;

  // The algorithm ensures that (a == -1 or aSrc[a] <= key) and (b == count or key < aSrc[b]),
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
  return a;
}

}
