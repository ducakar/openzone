/*
 *  common.h
 *
 *  Common types and function templates
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

/**
 *  \file common.h
 *
 *  \brief Common types and function templates
 *
 *  You may add <code>null</code> and the types to your
 *  <code>~/.kde/share/apps/katepart/syntax/cpp.xml</code> or global file
 *  <code>$KDEDIR/share/apps/katepart/syntax/cpp.xml</code> to look like reserved words in
 *  Katepart (Kate/KWrite/KDevelop).
 */

namespace oz
{

  /**
   * \def null
   * It is equivalent to NULL macro but it looks prettier.
   */
# define null 0

  /**
   * signed char
   * It should be used where char must be signed (otherwise char may be either signed or unsigned
   * depeneding on the platform).
   */
  typedef signed   char  byte;

  /**
   * unsigned char
   * It should be used where char must be unsigned (otherwise char may be either signed or unsigned
   * depeneding on the platform).
   */
  typedef unsigned char  ubyte;

  /**
   * unsigned short integer
   */
  typedef unsigned short ushort;

  /**
   * unsigned integer
   */
  typedef unsigned int   uint;

  /**
   * unsigned long integer
   */
  typedef unsigned long  ulong;

  /**
   * \def $
   * Embed identifier as string
   * E.g. if we have a variable named var return a string "var".
   */
// # define $( s ) #s

  /*
   * MISCELLANEOUS TEMPLATES
   */

  /**
   * Swap values of a and b.
   * @param a reference to first variable
   * @param b reference to second variable
   */
  template <class ValueA, class ValueB>
  inline void swap( ValueA &a, ValueB &b )
  {
    ValueA temp = a;

    a = b;
    b = temp;
  }

  /**
   * Absolute value.
   * @param a
   * @return absolute value
   */
  template <class Value>
  inline Value abs( const Value &a )
  {
    return a < 0 ? -a : a;
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return minimum of a and b
   */
  template <class Value, class ValueB>
  inline const Value &min( const Value &a, const ValueB &b )
  {
    return a < b ? a : b;
  }

  /**
   * Maximum.
   * @param a
   * @param b
   * @return maximum of a and b
   */
  template <class Value, class ValueB>
  inline const Value &max( const Value &a, const ValueB &b )
  {
    return a > b ? a : b;
  }

  /**
   * c bounded between a and b. Equals to max( min( c, b ), a ).
   * @param c
   * @param a
   * @param b
   * @return clamped value of c
   */
  template <class ValueC, class ValueA, class ValueB>
  inline const ValueC &bound( const ValueC &c, const ValueA &a, const ValueB &b )
  {
    assert( a < b );

    if( a > c ) {
      return a;
    }
    else if( b < c ) {
      return b;
    }
    else {
      return c;
    }
  }

  /*
   * ITERATOR TEMPLATES
   */

  /**
   * Generalized iterator.
   * It should only be used as a base class. Following functions need to be implemented:<br>
   * <code>bool isPassed()</code><br>
   * <code>Iterator &operator ++ ()</code><br>
   * <code>Iterator &operator -- ()</code> (optional)<br>
   * and, of course, a sensible constructor.
   */
  template <class Type>
  class IteratorBase
  {
    protected:

      /**
       * Element which iterator is currently positioned at.
       */
      Type *elem;

      /**
       * @param start first element
       */
      explicit IteratorBase( Type *start ) : elem( start )
      {}

    public:

      /**
       * Returns true if iterator is on specified element.
       * @param e
       * @return
       */
      bool operator == ( const Type *e ) const
      {
        return elem == e;
      }

      /**
       * Returns true if iterator is not on specified element.
       * @param e
       * @return
       */
      bool operator != ( const Type *e ) const
      {
        return elem != e;
      }

      /**
       * @return pointer to current element
       */
      operator Type* ()
      {
        return elem;
      }

      /**
       * @return constant pointer to current element
       */
      operator const Type* () const
      {
        return elem;
      }

      /**
       * @return reference to current element
       */
      Type &operator * ()
      {
        return *elem;
      }

      /**
       * @return constant reference to current element
       */
      const Type &operator * () const
      {
        return *elem;
      }

      /**
       * @return non-constant access to member
       */
      Type *operator -> ()
      {
        return elem;
      }

      /**
       * @return constant access to member
       */
      const Type *operator -> () const
      {
        return elem;
      }

  };

  /**
   * Pointer iterator
   */
  template <class Type>
  class Iterator : public IteratorBase<Type>
  {
    private:

      // base class
      typedef IteratorBase<Type> B;

    protected:

      /**
       * Successor of the last element.
       * Is is used to determine when iterator becomes invalid.
       */
      const Type *past;

    public:

      /**
       * @param start first element for forward iterator or successor of last element for backward
       * iterator
       * @param past_ successor of last element for forward iterator or predecessor of first element
       * for backward iterator
       */
      explicit Iterator( Type *start, const Type *past_ ) : B( start ), past( past_ )
      {}

      /**
       * When iterator advances beyond last element, it becomes passed. It points to an invalid
       * location.
       * @return true if iterator is passed
       */
      bool isPassed() const
      {
        return B::elem == past;
      }

      /**
       * Advance to next element.
       */
      Iterator &operator ++ ()
      {
        assert( B::elem != past );

        B::elem++;
        return *this;
      }

      /**
       * Advance to previous element.
       */
      Iterator &operator -- ()
      {
        assert( B::elem != past );

        B::elem--;
        return *this;
      }

  };

  /*
   * ARRAY UTILIY TEMPLATES
   */

  /**
   * Set array members to given value (memset).
   * In contrast with memset it calls constructor/destructor/assign operator on objects.
   * @param dest pointer to the first element
   * @param value value to be set
   * @param count number of elements to be set
   */
  template <class Type, class Value>
  inline void aSet( Type *dest, const Value &value, int count )
  {
    for( int i = 0; i < count; i++ ) {
      dest[i] = value;
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
    for( int i = count - 1; i >= 0; i-- ) {
      dest[i] = src[i];
    }
  }

  /**
   * Find the first occurence of an element.
   * @param array pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurence, -1 if not found
   */
  template <class Type, class Value>
  inline int aIndex( const Type *array, int count, const Value &value )
  {
    for( int i = 0; i < count; i++ ) {
      if( array[i] == value ) {
        return i;
      }
    }
    return -1;
  }

  /**
   * Find the last occurence of an element.
   * @param array pointer to the first element in the array
   * @param count number of elements to be looked upon
   * @param value value we look for
   * @return index of the first occurence, -1 if not found
   */
  template <class Type, class Value>
  inline int aLastIndex( const Type *array, int count, const Value &value )
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
    // 8-14 seem as optimal tresholds for switching to selection sort
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

  /*
   * ITERABLE CONTAINER UTILITY TEMPLATES
   */

  /**
   * \def foreach
   * Foreach macro can be used as in following example:
   * <pre>
   * Vector&lt;int&gt; v;
   * foreach( i, v.iterator() ) {
   *   printf( "%d ", *i );
   * }</pre>
   * There's no need to add it to Katepart syntax highlighting as it is already there.
   */
# define foreach( i, startIterator ) \
  for( typeof( startIterator ) i( startIterator ); !i.isPassed(); ++i )

  /**
   * Compare all elements. (Like STL equal)
   * @param iA
   * @param iB
   * @return true if all elements are equal
   */
  template <class IteratorA, class IteratorB>
  inline bool iEquals( IteratorA &iDest, IteratorB &iSrc )
  {
    while( !iDest.isPassed() ) {
      if( *iDest != *iSrc ) {
        return false;
      }
      iDest++;
      iSrc++;
    }
    return true;
  }

  /**
   * Set all elements. (Like STL fill)
   * @param i
   * @param value
   */
  template <class Iterator, class Value>
  inline void iSet( Iterator &i, Value value )
  {
    while( !i.isPassed() ) {
      *i = value;
      i++;
    }
  }

  /**
   * Copy elements from first to last. (Like STL copy)
   * @param iA
   * @param iB
   */
  template <class IteratorA, class IteratorB>
  inline void iCopy( IteratorA &iDest, IteratorB &iSrc )
  {
    while( !iDest.isPassed() ) {
      *iDest = *iSrc;
      iDest++;
      iSrc++;
    }
  }

  /**
   * Copy elements from last to first.
   * @param iA
   * @param iB
   */
  template <class BackwardIteratorA, class BackwardIteratorB>
  inline void iReverseCopy( BackwardIteratorA &iDest, BackwardIteratorB &iSrc )
  {
    while( !iDest.isPassed() ) {
      iDest--;
      iSrc--;
      *iDest = *iSrc;
    }
  }

  /**
   * Find first occurence of given element. (Like STL find)
   * @param i
   * @param value
   * @return iterator at the elements found, passed iterator if not found
   */
  template <class Iterator, class Value>
  inline Iterator iIndex( Iterator &i, Value value )
  {
    while( !i.isPassed() ) {
      if( *i == value ) {
        break;
      }
      i++;
    }
    return i;
  }

  /**
   * Find last occurence of given element.
   * @param i
   * @param value
   * @return iterator at the elements found, passed iterator if not found
   */
  template <class BackwardIterator, class Value>
  inline BackwardIterator iLastIndex( BackwardIterator &i, Value value )
  {
    while( !i.isPassed() ) {
      i--;
      if( *i == value ) {
        break;
      }
    }
    return i;
  }

  /**
   * Call delete on all elements (that have been previously allocated with the new call).
   * @param i
   */
  template <class Iterator, class Type>
  inline void iFree( Iterator &i )
  {
    while( !i.isPassed() ) {
      Type *p = *i;
      i++;
      delete p;
    }
  }

  /*
   * EXCEPTION CLASS
   */

  struct Exception
  {
    const char *message;
    const char *file;
    int        line;
    int        id;

    explicit Exception( int id_, const char *message_, const char *file_, int line_ ) :
        message( message_ ), file( file_ ), line( line_ ), id( id_ )
    {}
  };

#define Exception( id, message ) \
  Exception( ( id ), ( message ), __FILE__, __LINE__ )

}
