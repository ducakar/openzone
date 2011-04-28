/*
 *  DList.hpp
 *
 *  Double-linked list
 *  The Type should provide the "prev[INDEX]" and "next[INDEX]" pointers.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "iterables.hpp"

namespace oz
{

  /**
   * Double-linked list
   *
   * It can only be applied on classes that have <code>next[]</code> and <code>prev[]</code>
   * members.
   * Example:
   * <pre>struct C
   * {
   *   C* prev[2];
   *   C* next[2];
   *   int value;
   * };
   * ...
   * DList&lt;C, 0&gt; list1;
   * DList&lt;C, 1&gt; list2;</pre>
   * That way the objects of the same class can be in two separate lists at once.
   * <code>prev[0]</code> and <code>next[0]</code> point to previous and next element respectively
   * in <code>list1</code> and
   * <code>prev[1]</code> and <code>next[1]</code> point to previous and next element respectively
   * in <code>list2</code>.
   *
   * <code>prev[INDEX]</code> and <code>next[INDEX]</code> pointers are not cleared when element is
   * removed from the list, they may still point to elements in the list or to invalid locations!
   *
   * <code>DList</code> class doesn't take care of memory management except for the
   * <code>free()</code> method.
   *
   * <code>DList</code> is not a real container but merely binds together already existing elements.
   * So, copy operator does not copy the elements, to make a copy of a list including its elements,
   * use <code>DList&lt;Type&gt; copy = source.clone()</code>.
   */
  template <class Type, int INDEX = 0>
  class DList
  {
    public:

      /**
       * Constant DList iterator.
       */
      class CIterator : public CIteratorBase<Type>
      {
        private:

          typedef CIteratorBase<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          CIterator() : B( null )
          {}

          /**
           * Make iterator for given list. After creation it points to first element.
           * @param l
           */
          OZ_ALWAYS_INLINE
          explicit CIterator( const DList& l ) : B( l.firstElem )
          {}

          /**
           * Advance to the next element.
           * @return
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
       * DList iterator.
       */
      class Iterator : public IteratorBase<Type>
      {
        private:

          typedef IteratorBase<Type> B;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          Iterator() : B( null )
          {}

          /**
           * Make iterator for given list. After creation it points to first element.
           * @param l
           */
          OZ_ALWAYS_INLINE
          explicit Iterator( const DList& l ) : B( l.firstElem )
          {}

          /**
           * Advance to the next element.
           * @return
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

      // First element in list.
      Type* firstElem;
      // Last element in list.
      Type* lastElem;

    public:

      /**
       * Create an empty list.
       */
      DList() : firstElem( null ), lastElem( null )
      {}

      /**
       * Initialise a chain from an initialiser list of pointers.
       * @param l
       */
      DList( initializer_list<Type*> l )
      {
        firstElem = *l.begin();
        firstElem->prev[INDEX] = null;

        Type* prev = firstElem;

        for( Type* const* i = l.begin() + 1; i != l.end(); ++i ) {
          Type* curr = *i;

          prev->next[INDEX] = curr;
          curr->prev[INDEX] = prev;
          prev = curr;
        }

        prev->next[INDEX] = null;
        lastElem = prev;
      }

      /**
       * Copy a chain from an initialiser list of pointers.
       * @param l
       * @return
       */
      DList& operator = ( initializer_list<Type*> l )
      {
        firstElem = *l.begin();
        firstElem->prev[INDEX] = null;

        Type* prev = firstElem;

        for( Type* const* i = l.begin() + 1; i != l.end(); ++i ) {
          Type* curr = *i;

          prev->next[INDEX] = curr;
          curr->prev[INDEX] = prev;
          prev = curr;
        }

        prev->next[INDEX] = null;
        lastElem = prev;
      }

      /**
       * Clone list.
       * Create a new list from copies of all elements of the original list.
       * @return
       */
      DList clone() const
      {
        DList clone;
        Type* prev = null;
        Type* elem = firstElem;

        while( elem != null ) {
          Type* last = new Type( *elem );
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
       * Compare all elements in two lists.
       * Type should implement operator ==, otherwise comparison doesn't make sense (two copies
       * always differ on next[INDEX] members).
       * @param l
       * @return
       */
      bool equals( const DList& l ) const
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
       * @return constant iterator for this list
       */
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this list
       */
      OZ_ALWAYS_INLINE
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
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        hard_assert( ( firstElem == null ) == ( lastElem == null ) );

        return firstElem == null;
      }

      /**
       * @return constant pointer to first element in the list
       */
      OZ_ALWAYS_INLINE
      const Type* first() const
      {
        return firstElem;
      }

      /**
       * @return pointer to first element in the list
       */
      OZ_ALWAYS_INLINE
      Type* first()
      {
        return firstElem;
      }

      /**
       * @return constant pointer to last element in the list
       */
      OZ_ALWAYS_INLINE
      const Type* last() const
      {
        return lastElem;
      }

      /**
       * @return pointer to last element in the list
       */
      OZ_ALWAYS_INLINE
      Type* last()
      {
        return lastElem;
      }

      /**
       * @param e requested element
       * @return true if some element in the list points to the requested element
       */
      bool contains( const Type* e ) const
      {
        hard_assert( e != null );

        Type* p = firstElem;

        while( p != null ) {
          if( p == e ) {
            return true;
          }
          p = p->next[INDEX];
        }
        return false;
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void add( Type* e )
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
       * Insert an element after an element in the list.
       * @param e element to be inserted
       * @param p pointer to element after which we want to insert
       */
      void insertAfter( Type* e, Type* p )
      {
        hard_assert( e != null );
        hard_assert( p != null );

        Type* next = p->next[INDEX];

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
       * Insert an element before an element in the list.
       * @param e element to be inserted
       * @param p pointer to element before which we want to insert
       */
      void insertBefore( Type* e, Type* p )
      {
        hard_assert( e != null );
        hard_assert( p != null );

        Type* prev = p->prev[INDEX];

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
       * Remove an element from the list.
       * @param e element to be removed
       */
      void remove( Type* e )
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
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void pushFirst( Type* e )
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
       * Add element to the end of the list.
       * @param e element to be added
       */
      void pushLast( Type* e )
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
       * Pop first element from the list.
       * @return pointer to the first element
       */
      Type* popFirst()
      {
        hard_assert( firstElem != null );

        Type* p = firstElem;

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
       * Pop last element from the list.
       * @param e pointer to the last element
       */
      Type* popLast()
      {
        hard_assert( lastElem != null );

        Type* p = lastElem;

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
       * Empty the list but don't delete the elements.
       */
      void clear()
      {
        firstElem = null;
        lastElem = null;
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
        lastElem = null;
      }

  };

}
