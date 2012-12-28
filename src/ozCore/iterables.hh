/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @li Default constructor that should create an invalid iterator and
 * @li `Iterator& operator ++ ()`.
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
     * Create an iterator pointing to the given element.
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
 * Copy all elements from `iSrc` to `iDest`.
 */
template <class IteratorA, class CIteratorB>
inline void iCopy( IteratorA iDest, CIteratorB iSrc )
{
  while( iSrc.isValid() ) {
    hard_assert( iDest.isValid() );

    *iDest = *iSrc;

    ++iDest;
    ++iSrc;
  }
}

/**
 * Move all elements from `iSrc` to `iDest`.
 */
template <class IteratorA, class IteratorB>
inline void iMove( IteratorA iDest, IteratorB iSrc )
{
  typedef typename IteratorB::ElemType ElemB;

  while( iSrc.isValid() ) {
    hard_assert( iDest.isValid() );

    *iDest = static_cast<ElemB&&>( *iSrc );

    ++iDest;
    ++iSrc;
  }
}

/**
 * %Set elements to the given value.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline void iFill( Iterator iDest, const Value& value )
{
  while( iDest.isValid() ) {
    *iDest = value;

    ++iDest;
  }
}

/**
 * Swap element of two same-length containers.
 */
template <class IteratorA, class IteratorB>
inline void iSwap( IteratorA iDestA, IteratorB iDestB )
{
  typedef typename IteratorA::ElemType ElemA;
  typedef typename IteratorB::ElemType ElemB;

  while( iDestA.isValid() ) {
    hard_assert( iDestB.isValid() );

    ElemA t = static_cast<ElemA&&>( *iDestA );
    *iDestA = static_cast<ElemB&&>( *iDestB );
    *iDestB = static_cast<ElemA&&>( t );

    ++iDestA;
    ++iDestB;
  }

  hard_assert( !iDestB.isValid() );
}

/**
 * True iff same length and respective elements are equal.
 */
template <class CIteratorA, class CIteratorB>
inline bool iEquals( CIteratorA iSrcA, CIteratorB iSrcB )
{
  hard_assert( static_cast<void*>( &iSrcA ) != static_cast<void*>( &iSrcB ) );

  while( iSrcA.isValid() && iSrcB.isValid() && *iSrcA == *iSrcB ) {
    ++iSrcA;
    ++iSrcB;
  }
  return !iSrcA.isValid() && !iSrcB.isValid();
}

/**
 * True iff the given value is found in the container.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline bool iContains( CIterator iSrc, const Value& value )
{
  while( iSrc.isValid() && !( *iSrc == value ) ) {
    ++iSrc;
  }
  return iSrc.isValid();
}

/**
 * %Iterator for the first occurrence or an invalid iterator if not found.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline Iterator iFind( Iterator iSrc, const Value& value )
{
  while( iSrc.isValid() && !( *iSrc == value ) ) {
    ++iSrc;
  }
  return iSrc;
}

/**
 * %Iterator for the last occurrence or an invalid iterator if not found.
 */
template <class Iterator, typename Value = typename Iterator::ElemType>
inline Iterator iFindLast( Iterator iSrc, const Value& value )
{
  Iterator lastOccurence;

  while( iSrc.isValid() ) {
    if( *iSrc == value ) {
      lastOccurence = iSrc;
    }

    ++iSrc;
  }
  return lastOccurence;
}

/**
 * Index of the first occurrence of the value or -1 if not found.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline int iIndex( CIterator iSrc, const Value& value )
{
  int index = 0;

  while( iSrc.isValid() && !( *iSrc == value ) ) {
    ++iSrc;
    ++index;
  }
  return !iSrc.isValid() ? -1 : index;
}

/**
 * Index of the last occurrence of the value or -1 if not found.
 */
template <class CIterator, typename Value = typename CIterator::ElemType>
inline int iLastIndex( CIterator iSrc, const Value& value )
{
  int index = 0;
  int lastIndex = -1;

  while( iSrc.isValid() ) {
    if( *iSrc == value ) {
      lastIndex = index;
    }

    ++iSrc;
    ++index;
  }
  return lastIndex;
}

/**
 * Delete objects referenced by elements and set all elements to `nullptr`.
 */
template <class Iterator>
inline void iFree( Iterator iDest )
{
  typedef typename Iterator::ElemType Elem;

  while( iDest.isValid() ) {
    Elem& elem = *iDest;
    ++iDest;

    delete elem;
    elem = nullptr;
  }
}

/**
 * @def foreach
 * Macro to shorten common foreach loops.
 *
 * It can be used like
 * @code
 * List<int> l;
 * foreach( i, l.citer() ) {
 *   printf( "%d ", *i );
 * }
 * @endcode
 * to replace a longer piece of code, like:
 * @code
 * List<int> l;
 * for( auto i = l.citer(); i.isValid(); ++i )
 *   printf( "%d ", *i );
 * }
 * @endcode
 */
#define foreach( i, iterator ) \
  for( auto i = iterator; i.isValid(); ++i )

}
