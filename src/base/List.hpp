/*
 *  List.h
 *
 *  Linked list
 *  The Type should provide the "next[INDEX]" pointer
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  /**
   * Linked list
   *
   * It can only be applied on classes that have <code>next[]</code> member.
   * Example:
   * <pre>class C
   * {
   *   C *next[2];
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
   * In general all operations are O(1) except <code>contains()</code>, <code>length()</code> and
   * <code>free()</code> are O(n).
   */
  template <class Type, int INDEX>
  class List
  {
    public:

      /**
       * List iterator.
       */
      class Iterator : public IteratorBase<Type>
      {
        private:

          // base class
          typedef IteratorBase<Type> B;

        public:

          /**
           * Make iterator for given list. After creation it points to first element.
           * @param l
           */
          explicit Iterator( const List &l ) : B( l.firstElem )
          {}

          /**
           * When iterator advances beyond last element, it becomes passed. It points to an invalid
           * location.
           * @return true if iterator is passed
           */
          bool isPassed()
          {
            return B::elem == null;
          }

          /**
           * Advance to next element.
           */
          void operator ++ ( int )
          {
            assert( B::elem != null );

            B::elem = B::elem->next[INDEX];
          }

      };

    private:

      // First element in list.
      Type *firstElem;

      // No copying.
      List( const List& );
      List &operator = ( const List& );

    public:

      /**
       * Create an empty list.
       */
      List() : firstElem( null )
      {}

      /**
       * Create a list with only one element.
       * @param e the element
       */
      explicit List( Type *e ) : firstElem( e )
      {
        e->next[INDEX] = null;
      }

      /**
       * @return iterator for this list
       */
      Iterator iterator() const
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
        Type *p = firstElem;

        while( p != null ) {
          p = p->next[INDEX];
          i++;
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
      bool contains( const Type *e ) const
      {
        assert( e != null );

        Type *p = firstElem;

        while( p != null ) {
          if( p == e ) {
            return true;
          }
          p = p->next[INDEX];
        }
        return false;
      }

      /**
       * @return pointer to first element in the list
       */
      Type *first()
      {
        return firstElem;
      }

      /**
       * @return constant pointer to first element in the list
       */
      const Type *first() const
      {
        return firstElem;
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void operator << ( Type *e )
      {
        pushFirst( e );
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void add( Type *e )
      {
        pushFirst( e );
      }

      /**
       * Add element to the beginning of the list.
       * @param e element to be added
       */
      void pushFirst( Type *e )
      {
        assert( e != null );

        e->next[INDEX] = firstElem;
        firstElem = e;
      }

      void operator >> ( Type *&e )
      {
        e = popFirst();
      }

      /**
       * Pop first element from the list.
       * @param e reference to pointer where the pointer to the first element is to be saved
       */
      Type *popFirst()
      {
        assert( firstElem != null );

        Type *p = firstElem;

        firstElem = p->next[INDEX];
        return p;
      }

      /**
       * Insert an element after an element in the list.
       * @param e element to be inserted
       * @param p pointer to element after which we want to insert
       */
      void insertAfter( Type *e, Type *p )
      {
        assert( e != null );
        assert( p != null );

        e->next[INDEX] = p->next[INDEX];
        p->next[INDEX] = e;
      }

      /**
       * Remove an element from the list. Becouse we don't have double-linked list, you have to
       * provide pointer to previous element.
       * @param e element to be removed
       * @param prev previous element
       */
      void remove( Type *e, Type *prev )
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
       * Empty the list but don't delete the elements.
       */
      void clear()
      {
        firstElem = null;
      }

      /**
       * Empty the list and delete all elements - take care of memory managment.
       */
      void free()
      {
        Type *p = firstElem;

        while( p != null ) {
          Type *next = p->next[INDEX];

          delete p;
          p = next;
        }
        firstElem = null;
      }

  };

}
