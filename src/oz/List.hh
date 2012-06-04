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
 * @file oz/List.hh
 *
 * List template class.
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
 * @code
 * struct C
 * {
 *   C*  next[2];
 *   int value;
 * };
 * ...
 * List<C, 0> list1;
 * List<C, 1> list2;
 * @endcode
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
     *
     * For efficiency reasons, elements are added to the beginning of a list in contrast with
     * vector.
     */
    void add( Elem* e )
    {
      pushFirst( e );
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
     * Remove the first element from the list.
     *
     * To keep LIFO behaviour for <tt>add()</tt> and <tt>remove()</tt> methods like in vector, the
     * first element is removed instead of the last one.
     */
    void remove()
    {
      popFirst();
    }

    /**
     * Remove an element from the list.
     *
     * Because this list is not a double-linked, one have to provide pointer to the preceding element.
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

      Elem* e = firstElem;

      firstElem = firstElem->next[INDEX];
      return e;
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
