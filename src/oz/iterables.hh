/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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
 * <ul>
 *   <li><tt>bool isValid() const</tt> (if necessary)</li>
 *   <li><tt>CIterator& operator ++ ()</tt></li>
 * </ul>
 * and constructors of course.
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
 * <ul>
 *   <li><tt>bool isValid() const</tt> (if necessary)</li>
 *   <li><tt>Iterator& operator ++ ()</tt></li>
 * </ul>
 * and a constructor of course.
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
 * @def OZ_RANGE_ITERATOR
 * Define wrapper for range-for loop inside an iterator class.
 *
 * Note that in contrast with STL iterators, <tt>RangeIterator</tt> evaluates to the iterator that
 * points to the current element rather than the current element itself. That means that the rough
 * equivalent for STL code
 * <pre>
 * std\::vector\<int\> v;
 * for( auto& i : v ) {
 *   printf( "%d\n", i );
 * }
 * </pre>
 * is the following code
 * <pre>
 * Vector\<int\> v;
 * for( auto i : v.citer() ) {
 *   printf( "%d\n", *i );
 * }
 * </pre>
 * It's a little longer, but we don't loose access to the original iterator, which is very useful
 * with <tt>Map</tt>, <tt>HashIndex</tt> and <tt>HashString</tt> to access the current element's
 * value. Furthermore, one also explicitly specifies whether access to the container elements is
 * constant or non-constant.
 *
 * @ingroup oz
 */
#define OZ_RANGE_ITERATOR( Iterator ) \
  public: \
    class RangeIterator \
    { \
      friend RangeIterator begin<Iterator>( Iterator& iter ); \
      friend RangeIterator end<Iterator>( Iterator& iter ); \
      private: \
        Iterator& iter; \
        OZ_ALWAYS_INLINE \
        explicit RangeIterator( Iterator& iter_ ) : iter( iter_ ) {} \
      public: \
        OZ_ALWAYS_INLINE \
        bool operator != ( const RangeIterator& ) const { return iter.isValid(); } \
        OZ_ALWAYS_INLINE \
        Iterator& operator * () { return iter; } \
        OZ_ALWAYS_INLINE \
        RangeIterator& operator ++ () { ++iter; return *this; } \
    };

/**
 * <tt>begin()</tt> template for range-for.
 *
 * @ingroup oz
 */
template <class Iterator>
OZ_ALWAYS_INLINE
inline typename Iterator::RangeIterator begin( Iterator& iter )
{
  return typename Iterator::RangeIterator( iter );
}

/**
 * <tt>end()</tt> template for range-for.
 *
 * @ingroup oz
 */
template <class Iterator>
OZ_ALWAYS_INLINE
inline typename Iterator::RangeIterator end( Iterator& iter )
{
  return typename Iterator::RangeIterator( iter );
}

/**
 * @def foreach
 * Foreach macro.
 *
 * An alternative to range-for loop, may be more suitable for iterator concept used in this library.
 * It doesn't need <tt>RangeIterator</tt> wrapper to work which may result in better performance
 * when compiling without optimisation. Furthermore, it works fine with auto-completion and syntax
 * checker in KDevelop and Eclipse CDT.
 *
 * It can be used like
 * <pre>
 * Vector\<int\> v;
 * foreach( i, v.citer() ) {
 *   printf( "%d ", *i );
 * }
 * </pre>
 * to replace a longer piece of code, like:
 * <pre>
 * Vector\<int\> v;
 * for( auto i = v.citer(); i.isValid(); ++i )
 *   printf( "%d ", *i );
 * }
 * </pre>
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
  hard_assert( iSrcA != iSrcB );

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
 * %Iterator to the first element with the given value or an invalid iterator if not found.
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
 * %Iterator to the last element with the given value or an invalid iterator if not found.
 *
 * @ingroup oz
 */
template <class CIterator, typename Value>
inline CIterator iFindLast( CIterator iSrc, const Value& value )
{
  // Default constructor creates an invalid, passed iterator.
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
