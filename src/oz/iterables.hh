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
 * @file oz/iterables.hh
 *
 * %Iterator base classes and utility functions for iterable containers.
 *
 * For all functions that work with iterators it is assumed that one uses them with iterators that
 * have not been incremented yet, so they point to the first element in a container.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Base class for iterators with constant access to container elements.
 *
 * It should only be used as a base class. Following functions need to be implemented:
 * @li <tt>bool isValid() const</tt> (if necessary)
 * @li <tt>CIterator& operator ++ ()</tt>.
 *
 * @ingroup oz
 */
template <typename Elem>
class CIteratorBase
{
  public:

    /// Element type.
    typedef Elem ElemType;

  protected:

    /// Pointer to the element iterator is currently pointing at.
    const Elem* elem;

    /**
     * Create an iterator that points to the given element.
     */
    OZ_ALWAYS_INLINE
    explicit CIteratorBase( const Elem* start ) :
      elem( start )
    {}

  public:

    /**
     * True while iterator has not passed all elements.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return elem != null;
    }

    /**
     * Constant pointer to the current element.
     */
    OZ_ALWAYS_INLINE
    operator const Elem* () const
    {
      return elem;
    }

    /**
     * Constant reference to the current element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator * () const
    {
      return *elem;
    }

    /**
     * Constant access to a current element's member.
     */
    OZ_ALWAYS_INLINE
    const Elem* operator -> () const
    {
      return elem;
    }

    /**
     * Advance to the next element, should be implemented in derived classes.
     */
    OZ_ALWAYS_INLINE
    CIteratorBase& operator ++ () = delete;

};

/**
 * Base class for iterators with non-constant access to container elements.
 *
 * It should only be used as a base class. Following functions need to be implemented:
 * @li <tt>bool isValid() const</tt> (if necessary)
 * @li <tt>Iterator& operator ++ ()</tt>.
 *
 * @ingroup oz
 */
template <typename Elem>
class IteratorBase
{
  public:

    /**
     * Element type
     */
    typedef Elem ElemType;

  protected:

    /**
     * Element which iterator is currently positioned at.
     */
    Elem* elem;

    /**
     * Create an iterator that points to the given element.
     */
    OZ_ALWAYS_INLINE
    explicit IteratorBase( Elem* start ) :
      elem( start )
    {}

  public:

    /**
     * True while iterator has not passed all elements.
     */
    OZ_ALWAYS_INLINE
    bool isValid() const
    {
      return elem != null;
    }

    /**
     * Constant pointer to the current element.
     */
    OZ_ALWAYS_INLINE
    operator const Elem* () const
    {
      return elem;
    }

    /**
     * Pointer to the current element.
     */
    OZ_ALWAYS_INLINE
    operator Elem* ()
    {
      return elem;
    }

    /**
     * Constant reference to the current element.
     */
    OZ_ALWAYS_INLINE
    const Elem& operator * () const
    {
      return *elem;
    }

    /**
     * Reference to the current element.
     */
    OZ_ALWAYS_INLINE
    Elem& operator * ()
    {
      return *elem;
    }

    /**
     * Constant access to a current element's member.
     */
    OZ_ALWAYS_INLINE
    const Elem* operator -> () const
    {
      return elem;
    }

    /**
     * Access to a current element's member.
     */
    OZ_ALWAYS_INLINE
    Elem* operator -> ()
    {
      return elem;
    }

    /**
     * Advance to the next element, should be implemented in derived classes.
     */
    OZ_ALWAYS_INLINE
    IteratorBase& operator ++ () = delete;

};

/**
 * @def foreach
 * Macro to shorten common foreach loops.
 *
 * It can be used like
 * @code
 * Vector<int> v;
 * foreach( i, v.citer() ) {
 *   printf( "%d ", *i );
 * }
 * @endcode
 * to replace a longer piece of code, like:
 * @code
 * Vector<int> v;
 * for( auto i = v.citer(); i.isValid(); ++i )
 *   printf( "%d ", *i );
 * }
 * @endcode
 *
 * @ingroup oz
 */
#define foreach( i, iterator ) \
  for( auto i = iterator; i.isValid(); ++i )

/**
 * Copy elements.
 *
 * @ingroup oz
 */
template <class IteratorA, class CIteratorB>
inline void iCopy( IteratorA iDest, CIteratorB iSrc )
{
  hard_assert( static_cast<void*>( &iDest ) != static_cast<void*>( &iSrc ) );

  while( iDest.isValid() ) {
    hard_assert( iSrc.isValid() );

    *iDest = *iSrc;
    ++iDest;
    ++iSrc;
  }
}

/**
 * Move elements.
 *
 * @ingroup oz
 */
template <class IteratorA, class IteratorB>
inline void iMove( IteratorA iDest, IteratorB iSrc )
{
  hard_assert( static_cast<void*>( &iDest ) != static_cast<void*>( &iSrc ) );

  typedef typename IteratorB::ElemType ElemB;

  while( iDest.isValid() ) {
    hard_assert( iSrc.isValid() );

    *iDest = static_cast<ElemB&&>( *iSrc );
    ++iDest;
    ++iSrc;
  }
}

/**
 * Set elements to the given value.
 *
 * @ingroup oz
 */
template <class Iterator, typename Value>
inline void iSet( Iterator iDest, const Value& value )
{
  while( iDest.isValid() ) {
    *iDest = value;
    ++iDest;
  }
}

/**
 * True iff same length and respective elements are equal.
 *
 * @ingroup oz
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
 *
 * @ingroup oz
 */
template <class CIterator, typename Value>
inline bool iContains( CIterator iSrc, const Value& value )
{
  while( iSrc.isValid() && !( *iSrc == value ) ) {
    ++iSrc;
  }
  return iSrc.isValid();
}

/**
 * %Iterator for the first occurrence or an invalid iterator if not found.
 *
 * @ingroup oz
 */
template <class Iterator, typename Value>
inline Iterator iFind( Iterator iSrc, const Value& value )
{
  while( iSrc.isValid() && !( *iSrc == value ) ) {
    ++iSrc;
  }
  return iSrc;
}

/**
 * %Iterator for the last occurrence or an invalid iterator if not found.
 *
 * @ingroup oz
 */
template <class Iterator, typename Value>
inline Iterator iFindLast( Iterator iSrc, const Value& value )
{
  // Default constructor creates an invalid, passed iterator.
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
 *
 * @ingroup oz
 */
template <class CIterator, typename Value>
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
 *
 * @ingroup oz
 */
template <class CIterator, typename Value>
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
 * Delete objects referenced by elements and set all elements to <tt>null</tt>.
 *
 * @ingroup oz
 */
template <class Iterator>
inline void iFree( Iterator iDest )
{
  typedef typename Iterator::ElemType Elem;

  while( iDest.isValid() ) {
    Elem& elem = *iDest;
    ++iDest;

    delete elem;
    elem = null;
  }
}

}
