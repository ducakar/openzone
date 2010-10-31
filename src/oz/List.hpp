/*
 *  List.hpp
 *
 *  Linked list
 *  The Type should provide the "next[INDEX]" pointer.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "iterables.hpp"

namespace oz
{

  /**
   * Linked list
   *
   * It can only be applied on classes that have <code>next[]</code> member.
   * Example:
   * <pre>struct C
   * {
   *   C* next[2];
   *   int value;
   * };
   * ...
   * List&lt;C, 0&gt; list1;
   * List&lt;C, 1&gt; list2;</pre>
   * That way the objects of the same class can be in two separate lists at once.
   * <code>next[0]</code> points to next element in <code>list1</code> and
   * <code>next[1]</code> points to next element in <code>list2</code>.
   *
   * <code>next[INDEX]</code> pointer is not cleared when element is removed from the list,
   * it may still point to elements in the list or to invalid locations!
   *
   * <code>List</code> class doesn't take care of memory management except for the
   * <code>free()</code> method.
   *
   * <code>List</code> is not a real container but merely binds together already existing elements.
   * So, copy operator does not copy the elements, to make a copy of a list including its elements,
   * use <code>List&lt;Type&gt; copy = source.clone()</code>.
   */
  template <class Type, int INDEX = 0>
  class List
  {
    public:

      /**
       * Constant List iterator.
       */
      class CIterator : public CIteratorBase<Type>
      {
        private:

          typedef CIteratorBase<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          explicit CIterator() : B( null )
          {}

          /**
           * Make iterator for given list. After creation it points to first element.
           * @param l
           */
          explicit CIterator( const List& l ) : B( l.firstElem )
          {}

          /**
           * Advance to next element.
           */
          CIterator& operator ++ ()
          {
            assert( B::elem != null );

            B::elem = B::elem->next[INDEX];
            return *this;
          }

      };

      /**
       * List iterator.
       */
      class Iterator : public IteratorBase<Type>
      {
        private:

          typedef IteratorBase<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          explicit Iterator() : B( null )
          {}

          /**
           * Make iterator for given list. After creation it points to first element.
           * @param l
           */
          explicit Iterator( const List& l ) : B( l.firstElem )
          {}

          /**
           * Advance to next element.
           */
          Iterator& operator ++ ()
          {
            assert( B::elem != null );

            B::elem = B::elem->next[INDEX];
            return *this;
          }

      };

    private:

      // First element in list.
      Type* firstElem;

    public:

      /**
       * Create an empty list.
       */
      explicit List() : firstElem( null )
      {}

      /**
       * Clone list.
       * Create a new list from copies of all elements of the original list.
       * @return
       */
      List clone() const
      {
        List clone;
        Type* prev = null;
        Type* elem = firstElem;

        while( elem != null ) {
          Type* last = new Type( *elem );

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
       * Compare all elements in two lists.
       * Type should implement operator ==, otherwise comparison doesn't make sense (two copies
       * always differ on next[INDEX] members).
       * @param l
       * @return
       */
      bool operator == ( const List& l ) const
      {
        Type* e1 = firstElem;
        Type* e2 = l.firstElem;

        while( e1 != null && e2 != null && *e1 == *e2 ) {
          e1 = e1->next[INDEX];
          e2 = e2->next[INDEX];
        }
        return e1 == e2;
      }

      /**
       * Compare all elements in two lists.
       * Type should implement operator ==, otherwise comparison doesn't make sense (two copies
       * always differ on next[INDEX] members).
       * @param l
       * @return
       */
      bool operator != ( const List& l ) const
      {
        Type* e1 = firstElem;
        Type* e2 = l.firstElem;

        while( e1 != null && e2 != null && *e1 == *e2 ) {
          e1 = e1->next[INDEX];
          e2 = e2->next[INDEX];
        }
        return e1 != e2;
      }

      /**
       * @return constant iterator for this list
       */
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this list
       */
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * Count the elements in the list.
       * @return size of the list
       */
      int length() const
      {
        int i = 0;
        Type* p = firstElem;

        while( p != null ) {
          p = p->next[INDEX];
          ++i;
        }
        return i;
      }

      /**
       * @return true if the list has no elements
       */
      bool isEmpty() const
      {
        return firstElem == null;
      }

      /**
       * @param e requested element
       * @return true if some element in the list points to the requested element
       */
      bool contains( const Type* e ) const
      {
        assert( e != null );

        Type* p = firstElem;

        while( p != null && p != e ) {
          p = p->next[INDEX];
        }
        return p != null;
      }

      /**
       * @return constant pointer to first element in the list
       */
      const Type* first() const
      {
        return firstElem;
      }

      /**
       * @return pointer to first element in the list
       */
      Type* first()
      {
        return firstElem;
      }

      /**
       * @return constant pointer to last element in the list (linear time complexity)
       */
      const Type* last() const
      {
        const Type* last = firstElem;

        while( last != null ) {
          last = last->next[INDEX];
        }
        return last;
      }

      /**
       * @return pointer to last element in the list (linear time complexity)
       */
      Type* last()
      {
        Type* last = firstElem;

        while( last != null ) {
          last = last->next[INDEX];
        }
        return last;
      }

      /**
       * @return constant pointer to element before the given one
       */
      const Type* before( const Type* e ) const
      {
        Type* elem = firstElem;
        Type* beforeElem = null;

        while( last != e ) {
          beforeElem = elem;
          elem = elem->next[INDEX];
        }
        return beforeElem;
      }

      /**
       * @return pointer to element before the given one
       */
      Type* before( const Type* e )
      {
        Type* elem = firstElem;
        Type* beforeElem = null;

        while( last != e ) {
          beforeElem = elem;
          elem = elem->next[INDEX];
        }
        return beforeElem;
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void add( Type* e )
      {
        assert( e != null );

        e->next[INDEX] = firstElem;
        firstElem = e;
      }

      /**
       * Insert an element after an element in the list.
       * @param e element to be inserted
       * @param p pointer to element after which we want to insert
       */
      void insertAfter( Type* e, Type* p )
      {
        assert( e != null );
        assert( p != null );

        e->next[INDEX] = p->next[INDEX];
        p->next[INDEX] = e;
      }

      /**
       * Remove an element from the list. Because we don't have double-linked list, you have to
       * provide pointer to the previous element.
       * @param e element to be removed
       * @param prev previous element
       */
      void remove( Type* e, Type* prev )
      {
        assert( prev == null || prev->next[INDEX] == e );

        if( prev == null ) {
          firstElem = e->next[INDEX];
        }
        else {
          prev->next[INDEX] = e->next[INDEX];
        }
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void pushFirst( Type* e )
      {
        assert( e != null );

        e->next[INDEX] = firstElem;
        firstElem = e;
      }

      /**
       * Pop first element from the list.
       * @return the pointer to the first element
       */
      Type* popFirst()
      {
        assert( firstElem != null );

        Type* p = firstElem;

        firstElem = p->next[INDEX];
        return p;
      }

      /**
       * Transfer elements from given list. The given list is cleared after the operation.
       * @param l
       */
      void transfer( List& l )
      {
        if( l.isEmpty() ) {
          return;
        }

        l.last()->next[INDEX] = firstElem;
        firstElem = l.firstElem;

        firstElem = null;
      }

      /**
       * Empty the list but don't delete the elements.
       */
      void clear()
      {
        firstElem = null;
      }

      /**
       * Empty the list and delete all elements - take care of memory management.
       */
      void free()
      {
        Type* p = firstElem;

        while( p != null ) {
          Type* next = p->next[INDEX];

          delete p;
          p = next;
        }

        firstElem = null;
      }

  };

}
