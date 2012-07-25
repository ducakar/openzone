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
 * @file oz/DChain.hh
 *
 * DChain template class.
 */

#pragma once

#include "iterables.hh"

namespace oz
{

/**
 * Double-linked list.
 *
 * This is not a real container but merely a way of binding existing elements into a linked list.
 *
 * It can only be applied on classes that have <tt>prev[]</tt> and <tt>next[]</tt> members.
 * Example:
 * @code
 * struct C
 * {
 *   C*  prev[2];
 *   C*  next[2];
 *   int value;
 * };
 * ...
 * DChain<C, 0> chain1;
 * DChain<C, 1> chain2;
 * @endcode
 *
 * That way the objects of the same class can be in two separate chains at once;
 * <tt>prev[0]</tt> and <tt>next[0]</tt> point to previous and next element respectively
 * in <tt>chain1</tt> and
 * <tt>prev[1]</tt> and <tt>next[1]</tt> point to previous and next element respectively
 * in <tt>chain2</tt>.
 *
 * Notes:
 * \li Copy operations do not copy elements, to make a copy of a chain including its elements, use
 * <tt>clone()</tt> instead.
 * \li Removal operations (except for <tt>free()</tt> do not actually remove elements but only
 * decouples them from the chain.
 * \li <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> pointers are not cleared when an element is
 * removed from the chain, they may still point to elements in the chain or to invalid locations.
 *
 * @ingroup oz
 */
template <class Elem, int INDEX = 0>
class DChain
{
  public:

    /**
     * %Iterator with constant access to container elements.
     */
    class CIterator : public CIteratorBase<Elem>
    {
      friend class DChain;

      private:

        using CIteratorBase<Elem>::elem;

        /**
         * %Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator( const DChain& c ) :
          CIteratorBase<Elem>( c.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        CIterator() :
          CIteratorBase<Elem>( null )
        {}

        /**
         * Advance to the next element.
         */
        OZ_ALWAYS_INLINE
        CIterator& operator ++ ()
        {
          hard_assert( elem != null );

          elem = elem->next[INDEX];
          return *this;
        }

    };

    /**
     * %Iterator with non-constant access to container elements.
     */
    class Iterator : public IteratorBase<Elem>
    {
      friend class DChain;

      private:

        using IteratorBase<Elem>::elem;

        /**
         * %Iterator for the given container, points to the first element.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator( const DChain& c ) :
          IteratorBase<Elem>( c.firstElem )
        {}

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        Iterator() :
          IteratorBase<Elem>( null )
        {}

        /**
         * Advance to the next element.
         */
        OZ_ALWAYS_INLINE
        Iterator& operator ++ ()
        {
          hard_assert( elem != null );

          elem = elem->next[INDEX];
          return *this;
        }

    };

  private:

    Elem* firstElem; ///< Pointer to the first element in the chain.
    Elem* lastElem;  ///< Pointer to the last element in the chain.

  public:

    /**
     * Create an empty chain.
     */
    DChain() :
      firstElem( null ), lastElem( null )
    {}

    /**
     * Clone the chain.
     *
     * Create a new chain from copies of all elements of the original chain.
     */
    DChain clone() const
    {
      DChain clone;

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
     * <tt>Elem</tt> type should implement <tt>operator ==</tt>, otherwise comparison doesn't make
     * sense as two copies always differ in <tt>prev[INDEX]</tt> and <tt>next[INDEX]</tt> members.
     */
    bool equals( const DChain& c ) const
    {
      Elem* e1 = firstElem;
      Elem* e2 = c.firstElem;

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
     * Iterate through the chain and count elements.
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
     * True iff the chain has no elements.
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
    OZ_ALWAYS_INLINE
    Elem* last() const
    {
      return lastElem;
    }

    /**
     * True iff the given element is in the chain.
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
     * True iff an element equal to the given one is in the chain.
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
     * Add an element to the beginning of the chain.
     *
     * For efficiency reasons, elements are added to the beginning of a chain in contrast with array
     * chain.
     */
    void add( Elem* e )
    {
      pushFirst( e );
    }

    /**
     * Insert an element after some given element in the chain.
     */
    void insertAfter( Elem* e, Elem* p )
    {
      hard_assert( e != null && p != null );

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
     * Insert an element before some given element in the chain.
     */
    void insertBefore( Elem* e, Elem* p )
    {
      hard_assert( e != null && p != null );

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
     * Remove the first element from the chain.
     *
     * To keep LIFO behaviour for <tt>add()</tt> and <tt>remove()</tt> methods like in array lists,
     * the first element is removed instead of the last one.
     */
    void remove()
    {
      popFirst();
    }

    /**
     * Remove the given element from the chain.
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
     * Add an element to the beginning of the chain.
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
     * Add an element to the end of the chain.
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
     * Pop the first element from the chain.
     */
    Elem* popFirst()
    {
      hard_assert( firstElem != null );

      Elem* e = firstElem;

      firstElem = firstElem->next[INDEX];

      if( firstElem == null ) {
        lastElem = null;
      }
      else {
        firstElem->prev[INDEX] = null;
      }
      return e;
    }

    /**
     * Pop the last element from the chain.
     */
    Elem* popLast()
    {
      hard_assert( lastElem != null );

      Elem* e = lastElem;

      lastElem = lastElem->prev[INDEX];

      if( lastElem == null ) {
        firstElem = null;
      }
      else {
        lastElem->next[INDEX] = null;
      }
      return e;
    }

    /**
     * Empty the chain but do not delete the elements.
     */
    void clear()
    {
      firstElem = null;
      lastElem = null;
    }

    /**
     * Empty the chain and delete all elements.
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
