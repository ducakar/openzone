/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/iterables.hh
 *
 * %Iterator base classes and utility functions for iterable containers.
 *
 * For all functions that work with iterators it is assumed that one uses them with iterators that
 * have not been incremented yet, so they point to the first element in a container.
 *
 * @sa ozCore/arrays.hh
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Base class for iterators.
 *
 * It should only be used as a base class. The following functions have to be implemented:
 * - default constructor that creates an invalid iterator and
 * - `Iterator& operator ++ ()`.
 */
template <typename Elem>
class IteratorBase
{
public:

  /**
   * Element type.
   */
  typedef Elem ElemType;

protected:

  /**
   * Element which iterator is currently positioned at.
   */
  Elem* elem;

protected:

  /**
   * Create an iterator pointing to a given element.
   */
  OZ_ALWAYS_INLINE
  explicit IteratorBase( Elem* first ) :
    elem( first )
  {}

public:

  /**
   * True iff iterators point to the same element.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const IteratorBase& i ) const
  {
    return elem == i.elem;
  }

  /**
   * False iff iterators point to the same element.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const IteratorBase& i ) const
  {
    return elem != i.elem;
  }

  /**
   * True as long as iterator has not passed all elements.
   */
  OZ_ALWAYS_INLINE
  bool isValid() const
  {
    return elem != nullptr;
  }

  /**
   * Pointer to the current element.
   */
  OZ_ALWAYS_INLINE
  operator Elem* () const
  {
    return elem;
  }

  /**
   * Reference to the current element.
   */
  OZ_ALWAYS_INLINE
  Elem& operator * () const
  {
    return *elem;
  }

  /**
   * Access to the current element's member.
   */
  OZ_ALWAYS_INLINE
  Elem* operator -> () const
  {
    return elem;
  }

  /**
   * Advance to the next element, should be implemented in derived classes.
   */
  IteratorBase& operator ++ () = delete;

  /**
   * STL-style begin iterator, should be implemented in derived classes.
   */
  IteratorBase begin() const = delete;

  /**
   * STL-style end iterator, should be implemented in derived classes.
   */
  IteratorBase end() const = delete;

};

/**
 * Iterator with constant element access for a container (same as `container.citer()`).
 */
template <class Container>
OZ_ALWAYS_INLINE
inline typename Container::CIterator citer( const Container& container )
{
  return container.citer();
}

/**
 * Iterator with non-constant element access for a container (same as `container.iter()`).
 */
template <class Container>
OZ_ALWAYS_INLINE
inline typename Container::Iterator iter( Container& container )
{
  return container.iter();
}

/**
 * Count elements.
 */
template <class CIterator>
inline int iLength( CIterator iter )
{
  int count = 0;

  while( iter.isValid() ) {
    ++count;
    ++iter;
  }
  return count;
}

/**
 * Copy all elements from `srcIter` to `destIter`.
 */
template <class CIteratorA, class IteratorB>
inline void iCopy( CIteratorA srcIter, IteratorB destIter )
{
  while( srcIter.isValid() ) {
    hard_assert( destIter.isValid() );

    *destIter = *srcIter;

    ++srcIter;
    ++destIter;
  }
}

/**
 * Move all elements from `srcIter` to `destIter`.
 */
template <class IteratorA, class IteratorB>
inline void iMove( IteratorA srcIter, IteratorB destIter )
{
  typedef typename IteratorB::ElemType ElemB;

  while( srcIter.isValid() ) {
    hard_assert( destIter.isValid() );

    *destIter = static_cast<ElemB&&>( *srcIter );

    ++srcIter;
    ++destIter;
  }
}

/**
 * %Set elements to a given value.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline void iFill( Iterator iter, const Value& value )
{
  while( iter.isValid() ) {
    *iter = value;

    ++iter;
  }
}

/**
 * Swap element of two same-length containers.
 */
template <class IteratorA, class IteratorB>
inline void iSwap( IteratorA iterA, IteratorB iterB )
{
  typedef typename IteratorA::ElemType ElemA;
  typedef typename IteratorB::ElemType ElemB;

  while( iterA.isValid() ) {
    hard_assert( iterB.isValid() );

    ElemA t = static_cast<ElemA&&>( *iterA );
    *iterA = static_cast<ElemB&&>( *iterB );
    *iterB = static_cast<ElemA&&>( t );

    ++iterA;
    ++iterB;
  }

  hard_assert( !iterB.isValid() );
}

/**
 * True iff same length and respective elements are equal.
 */
template <class CIteratorA, class CIteratorB>
inline bool iEquals( CIteratorA iterA, CIteratorB iterB )
{
  hard_assert( static_cast<void*>( &iterA ) != static_cast<void*>( &iterB ) );

  while( iterA.isValid() && iterB.isValid() && *iterA == *iterB ) {
    ++iterA;
    ++iterB;
  }
  return !iterA.isValid() && !iterB.isValid();
}

/**
 * %Iterator for the first occurrence or an invalid iterator if not found.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline Iterator iFind( Iterator iter, const Value& value )
{
  while( iter.isValid() && !( *iter == value ) ) {
    ++iter;
  }
  return iter;
}

/**
 * %Iterator for the last occurrence or an invalid iterator if not found.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline Iterator iFindLast( Iterator iter, const Value& value )
{
  Iterator lastOccurence;

  while( iter.isValid() ) {
    if( *iter == value ) {
      lastOccurence = iter;
    }

    ++iter;
  }
  return lastOccurence;
}

/**
 * True iff a given value is found in a container.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline bool iContains( CIterator iter, const Value& value )
{
  return iFind<CIterator, Value>( iter, value ).isValid();
}

/**
 * Index of the first occurrence of the value or -1 if not found.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline int iIndex( CIterator iter, const Value& value )
{
  int index = 0;

  while( iter.isValid() && !( *iter == value ) ) {
    ++iter;
    ++index;
  }
  return !iter.isValid() ? -1 : index;
}

/**
 * Index of the last occurrence of the value or -1 if not found.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline int iLastIndex( CIterator iter, const Value& value )
{
  int index = 0;
  int lastIndex = -1;

  while( iter.isValid() ) {
    if( *iter == value ) {
      lastIndex = index;
    }

    ++iter;
    ++index;
  }
  return lastIndex;
}

/**
 * Delete objects referenced by elements and set all elements to `nullptr`.
 */
template <class Iterator>
inline void iFree( Iterator iter )
{
  typedef typename Iterator::ElemType Elem;

  while( iter.isValid() ) {
    Elem& elem = *iter;
    ++iter;

    delete elem;
    elem = nullptr;
  }
}

}
