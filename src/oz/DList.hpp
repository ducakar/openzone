/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/DList.hpp
 */

#pragma once

#include "iterables.hpp"

namespace oz
{

/**
 * Double-linked list.
 *
 * It can only be applied on classes that have <tt>next[]</tt> and <tt>prev[]</tt> members.
 * Example:
 * <pre>
 * struct C
 * {
 *   C*  prev[2];
 *   C*  next[2];
 *   int value;
 * };
 * ...
 * DList\<C, 0\> list1;
 * DList\<C, 1\> list2;
 * </pre>
 *
 * That way the objects of the same class can be in two separate lists at once.
 * <tt>prev[0]</tt> and <tt>next[0]</tt> point to previous and next element respectively
 * in <tt>list1</tt> and
 * <tt>prev[1]</tt> and <tt>next[1]</tt> point to previous and next element respectively
 * in <tt>list2</tt>.
 *
 * <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> pointers are not cleared when element is
 * removed from the list, they may still point to elements in the list or to invalid locations!
 *
 * <tt>%DList</tt> class doesn't take care of memory management except for the
 * <code>free()</code> method.
 *
 * <tt>%DList</tt> is not a real container but merely binds together already existing elements.
 * So, copy operator does not copy the elements, to make a copy of a list including its elements,
 * use <code>clone()</code> function.
 *
 * @ingroup oz
 */
template <class Elem, int INDEX = 0>
class DList
{
  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public CIteratorBase<Elem>
    {
      friend class DList;

      OZ_RANGE_ITERATOR( CIterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef CIteratorBase<Elem> B;

        /**
         * Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const DList& l ) : B( l.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() : B( null )
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
      friend class DList;

      OZ_RANGE_ITERATOR( Iterator )

      private:

        /// Base class type, convenience definition to make code cleaner.
        typedef IteratorBase<Elem> B;

        /**
         * Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const DList& l ) : B( l.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() : B( null )
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
    Elem* lastElem;  ///< Pointer to the last element in the list.

  public:

    /**
     * Create an empty list.
     */
    DList() : firstElem( null ), lastElem( null )
    {}

    /**
     * Clone the list.
     *
     * Create a new list from copies of all elements of the original list.
     */
    DList clone() const
    {
      DList clone;
      Elem* prev = null;
      Elem* elem = firstElem;

      while( elem != null ) {
        Elem* last = new Elem( *elem );
        last->prev[INDEX] = prev;

        if( prev == null ) {
          clone.firstElem = last;
        }
        else {
          prev->next[INDEX] = last;
        }
        prev = last;
      }
      clone.lastElem = prev;

      return clone;
    }

    /**
     * True iff same size and respective elements are equal.
     *
     * Elem should implement <tt>operator ==</tt>, otherwise comparison doesn't make sense as
     * two copies always differ in <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> members.
     */
    bool equals( const DList& l ) const
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
      hard_assert( ( firstElem == null ) == ( lastElem == null ) );

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
    OZ_ALWAYS_INLINE
    Elem* last() const
    {
      return lastElem;
    }

    /**
     * True iff the given element is in the list.
     */
    bool contains( const Elem* e ) const
    {
      hard_assert( e != null );

      Elem* p = firstElem;

      while( p != null ) {
        if( p == e ) {
          return true;
        }
        p = p->next[INDEX];
      }
      return false;
    }

    /**
     * Add an element to the beginning of the list.
     */
    void add( Elem* e )
    {
      hard_assert( e != null );

      e->prev[INDEX] = null;
      e->next[INDEX] = firstElem;

      if( firstElem == null ) {
        firstElem = e;
        lastElem = e;
      }
      else {
        firstElem->prev[INDEX] = e;
        firstElem = e;
      }
    }

    /**
     * Insert an element after some given element in the list.
     */
    void insertAfter( Elem* e, Elem* p )
    {
      hard_assert( e != null );
      hard_assert( p != null );

      Elem* next = p->next[INDEX];

      e->prev[INDEX] = p;
      e->next[INDEX] = p->next[INDEX];
      p->next[INDEX] = e;

      if( next == null ) {
        lastElem = e;
      }
      else {
        next->prev[INDEX] = e;
      }
    }

    /**
     * Insert an element before some given element in the list.
     */
    void insertBefore( Elem* e, Elem* p )
    {
      hard_assert( e != null );
      hard_assert( p != null );

      Elem* prev = p->prev[INDEX];

      e->next[INDEX] = p;
      e->prev[INDEX] = prev;
      p->prev[INDEX] = e;

      if( prev == null ) {
        firstElem = e;
      }
      else {
        p->prev[INDEX] = e;
      }
    }

    /**
     * Remove the last element from the list.
     */
    void remove()
    {
      hard_assert( lastElem != null );

      lastElem = lastElem->prev[INDEX];

      if( lastElem == null ) {
        firstElem = null;
      }
      else {
        lastElem->next[INDEX] = null;
      }
    }

    /**
     * Remove the given element from the list.
     */
    void remove( Elem* e )
    {
      if( e->prev[INDEX] == null ) {
        firstElem = e->next[INDEX];
      }
      else {
        e->prev[INDEX]->next[INDEX] = e->next[INDEX];
      }
      if( e->next[INDEX] == null ) {
        lastElem = e->prev[INDEX];
      }
      else {
        e->next[INDEX]->prev[INDEX] = e->prev[INDEX];
      }
    }

    /**
     * Add an element to the beginning of the list.
     */
    void pushFirst( Elem* e )
    {
      hard_assert( e != null );

      e->prev[INDEX] = null;
      e->next[INDEX] = firstElem;

      if( firstElem == null ) {
        firstElem = e;
        lastElem = e;
      }
      else {
        firstElem->prev[INDEX] = e;
        firstElem = e;
      }
    }

    /**
     * Add an element to the end of the list.
     */
    void pushLast( Elem* e )
    {
      hard_assert( e != null );

      e->prev[INDEX] = lastElem;
      e->next[INDEX] = null;

      if( lastElem == null ) {
        firstElem = e;
        lastElem = e;
      }
      else {
        lastElem->next[INDEX] = e;
        lastElem = e;
      }
    }

    /**
     * Pop the first element from the list.
     */
    Elem* popFirst()
    {
      hard_assert( firstElem != null );

      Elem* p = firstElem;

      firstElem = p->next[INDEX];

      if( firstElem == null ) {
        lastElem = null;
      }
      else {
        firstElem->prev[INDEX] = null;
      }
      return p;
    }

    /**
     * Pop the last element from the list.
     */
    Elem* popLast()
    {
      hard_assert( lastElem != null );

      Elem* p = lastElem;

      lastElem = p->prev[INDEX];

      if( lastElem == null ) {
        firstElem = null;
      }
      else {
        lastElem->next[INDEX] = null;
      }
      return p;
    }

    /**
     * Empty the list but do not delete the elements.
     */
    void clear()
    {
      firstElem = null;
      lastElem = null;
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
      lastElem = null;
    }

};

}
