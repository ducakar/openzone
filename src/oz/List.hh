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
 * @file oz/List.hh
 */

#pragma once

#include "iterables.hh"

namespace oz
{

/**
 * Linked list.
 *
 * It can only be applied on classes that have a <tt>next[]</tt> member.
 * Example:
 * <pre>
 * struct C
 * {
 *   C*  next[2];
 *   int value;
 * };
 * ...
 * List\<C, 0\> list1;
 * List\<C, 1\> list2;
 * </pre>
 *
 * That way the objects of the same class can be in two separate lists at once.
 * <tt>next[0]</tt> points to next element in <tt>list1</tt> and
 * <tt>next[1]</tt> points to next element in <tt>list2</tt>.
 *
 * <tt>next[INDEX]</tt> pointer is not cleared when element is removed from the list,
 * it may still point to elements in the list or to invalid locations!
 *
 * <tt>List</tt> is not a real container but merely binds together already existing elements.
 * So, copy operator does not copy the elements, to make a copy of a list including its elements,
 * use <tt>clone()</tt> instead. It also doesn't delete removed elements (except for <tt>free()</tt>
 * method).
 *
 * @ingroup oz
 */
template <class Elem, int INDEX = 0>
class List
{
  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public CIteratorBase<Elem>
    {
      friend class List;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef CIteratorBase<Elem> B;

        /**
         * %Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const List& l ) :
          B( l.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          B( null )
        {}

        /**
         * Advance to the next element.
         */
        OZ_ALWAYS_INLINE
        CIterator& operator ++ ()
        {
          hard_assert( B::elem != null );

          B::elem = B::elem->next[INDEX];
          return *this;
        }

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public IteratorBase<Elem>
    {
      friend class List;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef IteratorBase<Elem> B;

        /**
         * %Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const List& l ) :
          B( l.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          B( null )
        {}

        /**
         * Advance to the next element.
         */
        OZ_ALWAYS_INLINE
        Iterator& operator ++ ()
        {
          hard_assert( B::elem != null );

          B::elem = B::elem->next[INDEX];
          return *this;
        }

    };

  private:

    Elem* firstElem; ///< Pointer to the first element in the list.

  public:

    /**
     * Create an empty list.
     */
    List() :
      firstElem( null )
    {}

    /**
     * Clone the list.
     *
     * Create a new list from copies of all elements of the original list.
     */
    List clone() const
    {
      List clone;

      Elem* prev = null;
      Elem* elem = firstElem;

      while( elem != null ) {
        Elem* last = new Elem( *elem );

        if( prev == null ) {
          clone.firstElem = last;
        }
        else {
          prev->next[INDEX] = last;
        }
        prev = last;
      }

      return clone;
    }

    /**
     * True iff same size and respective elements are equal.
     *
     * <tt>Elem</tt> type should implement <tt>operator ==</tt>, otherwise comparison doesn't make
     * sense as two copies always differ in <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> members.
     */
    bool equals( const List& l ) const
    {
      Elem* e1 = firstElem;
      Elem* e2 = l.firstElem;

      while( e1 != null && e2 != null && *e1 == *e2 ) {
        e1 = e1->next[INDEX];
        e2 = e2->next[INDEX];
      }
      return e1 == e2;
    }

    /**
     * %Iterator with constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    CIterator citer() const
    {
      return CIterator( *this );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( *this );
    }

    /**
     * Number of elements.
     */
    int length() const
    {
      int i = 0;
      Elem* p = firstElem;

      while( p != null ) {
        p = p->next[INDEX];
        ++i;
      }
      return i;
    }

    /**
     * True iff the list has no elements.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return firstElem == null;
    }

    /**
     * Pointer to the first element.
     */
    OZ_ALWAYS_INLINE
    Elem* first() const
    {
      return firstElem;
    }

    /**
     * Pointer to the last element.
     */
    Elem* last() const
    {
      Elem* last = firstElem;

      while( last != null ) {
        last = last->next[INDEX];
      }
      return last;
    }

    /**
     * Pointer to the element before the given one.
     */
    Elem* before( const Elem* e ) const
    {
      Elem* current = firstElem;
      Elem* before = null;

      while( current != e ) {
        before = current;
        current = current->next[INDEX];
      }
      return before;
    }

    /**
     * True iff the given element is in the list.
     */
    bool has( const Elem* e ) const
    {
      hard_assert( e != null );

      Elem* p = firstElem;

      while( p != null && p != e ) {
        p = p->next[INDEX];
      }
      return p != null;
    }

    /**
     * True iff an element equal to the given one is in the list.
     *
     * <tt>Elem</tt> type should implement <tt>operator ==</tt>, otherwise comparison doesn't make
     * sense as two copies always differ in <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> members.
     */
    bool contains( const Elem* e ) const
    {
      hard_assert( e != null );

      Elem* p = firstElem;

      while( p != null && !( *p == *e ) ) {
        p = p->next[INDEX];
      }
      return p != null;
    }

    /**
     * Add an element to the beginning of the list.
     */
    void add( Elem* e )
    {
      hard_assert( e != null );

      e->next[INDEX] = firstElem;
      firstElem = e;
    }

    /**
     * Insert an element after some given element in the list.
     */
    void insertAfter( Elem* e, Elem* p )
    {
      hard_assert( e != null && p != null );

      e->next[INDEX] = p->next[INDEX];
      p->next[INDEX] = e;
    }

    /**
     * Remove an element from the list.
     *
     * Because this is not a double-linked list, one have to provide pointer to the element before.
     */
    void remove( Elem* e, Elem* prev )
    {
      hard_assert( prev == null || prev->next[INDEX] == e );

      if( prev == null ) {
        firstElem = e->next[INDEX];
      }
      else {
        prev->next[INDEX] = e->next[INDEX];
      }
    }

    /**
     * Add an element to the beginning of the list.
     */
    void pushFirst( Elem* e )
    {
      hard_assert( e != null );

      e->next[INDEX] = firstElem;
      firstElem = e;
    }

    /**
     * Pop the first element from the list.
     */
    Elem* popFirst()
    {
      hard_assert( firstElem != null );

      Elem* p = firstElem;

      firstElem = p->next[INDEX];
      return p;
    }

    /**
     * Empty the list but do not delete the elements.
     */
    void clear()
    {
      firstElem = null;
    }

    /**
     * Empty the list and delete all elements.
     */
    void free()
    {
      Elem* p = firstElem;

      while( p != null ) {
        Elem* next = p->next[INDEX];

        delete p;
        p = next;
      }

      firstElem = null;
    }

};

}
