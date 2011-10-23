/*
 *  iterables.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file iterables.hpp
 *
 * Iterator base classes and utility functions for iterable containers.
 *
 * For all functions that work with iterator it is assumed that one uses them with iterators that
 * have not been incremented yet, so they point to the first element in a container.
 */

#include "common.hpp"

namespace oz
{

/**
 * Base class for iterators with constant access to container elements.
 *
 * It should only be used as a base class. Following functions need to be implemented:<br>
 * <tt>bool isValid() const</tt> (if necessary)<br>
 * <tt>CIterator& operator ++ ()</tt><br>
 * and constructors of course.
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

    /// Create an iterator that points to the given element.
    OZ_ALWAYS_INLINE
    explicit CIteratorBase( const Elem* start ) : elem( start )
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

  private:

    /**
     * Advance to the next element.
     *
     * Should be implemented in derived classes.
     */
    OZ_ALWAYS_INLINE
    CIteratorBase& operator ++ ();

};

/**
 * Base class for iterators with non-constant access to container elements.
 *
 * It should only be used as a base class. Following functions need to be implemented:<br>
 * <tt>bool isValid() const</tt> (if necessary)<br>
 * <tt>Iterator& operator ++ ()</tt><br>
 * and a constructor of course.
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
     * @param start first element
     */
    OZ_ALWAYS_INLINE
    explicit IteratorBase( Elem* start ) : elem( start )
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

  private:

    /**
     * Advance to the next element.
     *
     * Should be implemented in derived classes.
     */
    OZ_ALWAYS_INLINE
    IteratorBase& operator ++ ();

};

/**
 * @def foreach
 *
 * Foreach loop.
 *
 * Foreach macro can be used as in following example:
 * <pre>
 * Vector\<int\>; v;
 * foreach( i, v.citer() ) {
 *   printf( "%d ", *i );
 * }
 * </pre>
 * This replaces a longer piece of code, like:
 * <pre>
 * Vector\<int\> v;
 * for( Vector\<int\>\::CIterator i = v.citer(); i.isValid(); ++i )
 *   printf( "%d ", *i );
 * }
 * </pre>
 */
# define foreach( i, iterator ) \
  for( auto i = iterator; i.isValid(); ++i )

/**
 * Copy elements.
 */
template <class IteratorA, class CIteratorB>
inline void iCopy( IteratorA iDest, CIteratorB iSrc )
{
  hard_assert( !iDest.isValid() || iDest != iSrc );

  while( iDest.isValid() ) {
    hard_assert( iSrc.isValid() );

    *iDest = *iSrc;
    ++iDest;
    ++iSrc;
  }
}

/**
 * Set elements to the given value.
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
 */
template <class CIteratorA, class CIteratorB>
inline bool iEquals( CIteratorA iSrcA, CIteratorB iSrcB )
{
  hard_assert( iSrcA != iSrcB );

  while( iSrcA.isValid() && iSrcB.isValid() && *iSrcA == *iSrcB ) {
    ++iSrcA;
    ++iSrcB;
  }
  return !iSrcA.isValid() && !iSrcB.isValid();
}

/**
 * True iff the given value is found in the container.
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
 * Iterator to the first element with the given value or an invalid iterator if not found.
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
 * Iterator to the last element with the given value or an invalid iterator if not found.
 */
template <class CIterator, typename Value>
inline CIterator iFindLast( CIterator iSrc, const Value& value )
{
  // default constructor produces an invalid, passed iterator
  CIterator lastOccurence;

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
