/*
 *  iterables.hpp
 *
 *  Basic iterator classes and utility templates.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  /**
   * Generalised constant iterator.
   * It should only be used as a base class. Following functions need to be implemented:<br>
   * <code>bool isPassed()</code><br>
   * <code>Iterator& operator ++ ()</code><br>
   * <code>Iterator& operator -- ()</code> (optional)<br>
   * and a constructor of course.
   */
  template <typename Type>
  class CIteratorBase
  {
    protected:

      /**
       * Element which iterator is currently positioned at.
       */
      const Type* elem;

      /**
       * @param start first element
       */
      explicit CIteratorBase( const Type* start ) : elem( start )
      {}

    public:

      /**
       * Returns true if the iterators point at the same element.
       * @param e
       * @return
       */
      bool operator == ( const CIteratorBase& i ) const
      {
        return elem == i.elem;
      }

      /**
       * Returns true if the iterators do not point at the same element.
       * @param e
       * @return
       */
      bool operator != ( const CIteratorBase& i ) const
      {
        return elem != i.elem;
      }

      /**
       * Returns true if the iterator is at the given element.
       * @param e
       * @return
       */
      bool operator == ( const Type* e ) const
      {
        return elem == e;
      }

      /**
       * Returns true if the iterator is not at the given element.
       * @param e
       * @return
       */
      bool operator != ( const Type* e ) const
      {
        return elem != e;
      }

      /**
       * Returns true when iterator goes past the last element.
       * May be overridden in derived classes
       * @return
       */
      bool isPassed() const
      {
        return elem == null;
      }

      /**
       * @return constant pointer to current element
       */
      operator const Type* () const
      {
        return elem;
      }

      /**
       * @return constant reference to current element
       */
      const Type& operator * () const
      {
        return *elem;
      }

      /**
       * @return constant access to member
       */
      const Type* operator -> () const
      {
        return elem;
      }

    private:

      /**
       * Advance to next element
       * Should be overridden in derived classes
       * @return
       */
      CIteratorBase& operator ++ ();

      /**
       * Go to previous element
       * May be overridden in derived classes (optional)
       * @return
       */
      CIteratorBase& operator -- ();

  };

  /**
   * Generalised iterator.
   * It should only be used as a base class. Following functions need to be implemented:<br>
   * <code>bool isPassed()</code> (if necessary)<br>
   * <code>Iterator& operator ++ ()</code><br>
   * <code>Iterator& operator -- ()</code> (optional)<br>
   * and a constructor of course.
   */
  template <typename Type>
  class IteratorBase
  {
    protected:

      /**
       * Element which iterator is currently positioned at.
       */
      Type* elem;

      /**
       * @param start first element
       */
      explicit IteratorBase( Type* start ) : elem( start )
      {}

    public:

      /**
       * Returns true if the iterators point at the same element.
       * @param e
       * @return
       */
      bool operator == ( const IteratorBase& i ) const
      {
        return elem == i.elem;
      }

      /**
       * Returns true if the iterators do not point at the same element.
       * @param e
       * @return
       */
      bool operator != ( const IteratorBase& i ) const
      {
        return elem != i.elem;
      }

      /**
       * Returns true if the iterator is at the given element.
       * @param e
       * @return
       */
      bool operator == ( const Type* e ) const
      {
        return elem == e;
      }

      /**
       * Returns true if the iterator is not at the given element.
       * @param e
       * @return
       */
      bool operator != ( const Type* e ) const
      {
        return elem != e;
      }

      /**
       * Returns true when iterator goes past the last element.
       * Should be overridden in derivative classes
       * @return
       */
      bool isPassed() const
      {
        return elem == null;
      }

      /**
       * @return constant pointer to current element
       */
      operator const Type* () const
      {
        return elem;
      }

      /**
       * @return pointer to current element
       */
      operator Type* ()
      {
        return elem;
      }

      /**
       * @return constant reference to current element
       */
      const Type& operator * () const
      {
        return *elem;
      }

      /**
       * @return reference to current element
       */
      Type& operator * ()
      {
        return *elem;
      }

      /**
       * @return constant access to member
       */
      const Type* operator -> () const
      {
        return elem;
      }

      /**
       * @return non-constant access to member
       */
      Type* operator -> ()
      {
        return elem;
      }

    private:

      /**
       * Advance to next element
       * Should be overridden in derivative classes
       * @return
       */
      IteratorBase& operator ++ ();

      /**
       * Go to previous element
       * May be overridden in derivative classes (optional)
       * @return
       */
      IteratorBase& operator -- ();

  };

  /**
   * \def foreach
   * Foreach macro can be used as in following example:
   * <pre>
   * Vector&lt;int&gt; v;
   * foreach( i, v.citer() ) {
   *   printf( "%d ", *i );
   * }</pre>
   * This replaces much more cryptic and longer pieces of code, like:
   * <pre>
   * Vector&lt;int&gt; v;
   * for( Vector&lt;int&gt;::Iterator i( v ); !i.isPassed(); ++i )
   *   printf( "%d ", *i );
   * }</pre>
   * There's no need to add it to Katepart syntax highlighting as it is already there (Qt has some
   * similar foreach macro).
   */
  # define foreach( i, iterator ) \
  for( auto i = iterator; !i.isPassed(); ++i )

  /**
   * Compare all elements (like std::equal, but containers have to be the same length).
   * @param iSrcA
   * @param iSrcB
   * @return true if all elements are equal and containers are the same length
   */
  template <class CIteratorA, class CIteratorB>
  inline bool iEquals( CIteratorA iSrcA, CIteratorB iSrcB )
  {
    assert( &*iSrcA != &*iSrcB );

    while( !iSrcA.isPassed() && !iSrcB.isPassed() && *iSrcA == *iSrcB ) {
      ++iSrcA;
      ++iSrcB;
    }
    return iSrcA.isPassed() && iSrcB.isPassed();
  }

  /**
   * Set all elements (like std::fill).
   * @param iDest
   * @param value
   */
  template <class Iterator, typename Value>
  inline void iSet( Iterator iDest, const Value& value )
  {
    while( !iDest.isPassed() ) {
      *iDest = value;
      ++iDest;
    }
  }

  /**
   * Copy elements from first to last (like std::copy, but reversed parameters).
   * @param iDest
   * @param iSrc
   */
  template <class IteratorA, class CIteratorB>
  inline void iCopy( IteratorA iDest, CIteratorB iSrc )
  {
    assert( &*iDest != &*iSrc );

    while( !iDest.isPassed() ) {
      assert( !iSrc.isPassed() );

      *iDest = *iSrc;
      ++iDest;
      ++iSrc;
    }
  }

  /**
   * Copy elements from last to first (like std::copy_backward, but reversed parameters).
   * @param iDest
   * @param iSrc
   */
  template <class ReverseIteratorA, class CReverseIteratorB>
  inline void iReverseCopy( ReverseIteratorA iDest, CReverseIteratorB iSrc )
  {
    assert( &*iDest != &*iSrc );

    while( !iDest.isPassed() ) {
      assert( !iSrc.isPassed() );

      *iDest = *iSrc;
      --iDest;
      --iSrc;
    }
  }

  /**
   * Return true if given value is found in container.
   * @param iSrc
   * @param value
   * @return
   */
  template <class CIterator, typename Value>
  inline bool iContains( CIterator iSrc, const Value& value )
  {
    while( !iSrc.isPassed() && *iSrc != value ) {
      ++iSrc;
    }
    return !iSrc.isPassed();
  }

  /**
   * Find first occurrence of given element (like std::find)
   * @param iSrc
   * @param value
   * @return iterator at the element found, passed iterator if not found
   */
  template <class CIterator, typename Value>
  inline CIterator iIndex( CIterator iSrc, const Value& value )
  {
    while( !iSrc.isPassed() && *iSrc != value ) {
      ++iSrc;
    }
    return iSrc;
  }

  /**
   * Find last occurrence of given element (like std::find_end).
   * @param iSrc
   * @param value
   * @return iterator at the element found, passed iterator if not found
   */
  template <class CReverseIterator, typename Value>
  inline CReverseIterator iLastIndex( CReverseIterator iSrc, const Value& value )
  {
    while( !iSrc.isPassed() && *iSrc != value ) {
      --iSrc;
    }
    return iSrc;
  }

  /**
   * Call delete on each non-null element of a container of pointers and set all elements to null.
   * @param iDest
   */
  template <class Iterator>
  inline void iFree( Iterator iDest )
  {
    while( !iDest.isPassed() ) {
      decltype( *iDest ) elem = *iDest;
      ++iDest;

      if( elem != null ) {
        delete elem;
        elem = null;
      }
    }
  }

}
