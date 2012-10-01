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
 * This is not a real container but a way of binding existing elements into a linked list.
 *
 * It can only be applied on classes that have `prev[]` and `next[]` members.
 * Example:
 * @code
 * struct C
 * {
 *   C*  prev[2];
 *   C*  next[2];
 *   int value;
 * };
 *
 * DChain<C, 0> chain1;
 * DChain<C, 1> chain2;
 *
 * C* c = new C();
 * chain1.add( c );
 * chain2.add( c );
 * @endcode
 * That way an objects can be in two separate chains at once;
 * `prev[0]` and `next[0]` point to previous and next element respectively in `chain1` and
 * `prev[1]` and `next[1]` point to previous and next element respectively in `chain2`.
 *
 * Notes:
 * @li Copy operations do not copy elements, to make a copy of a chain including its elements, use
 * `   clone()` instead.
 * @li Removal operations (except for `free()` do not actually remove elements but only decouples
 *     them from the chain.
 * @li `prev[INDEX]` and `next[INDEX]` pointers are not cleared when an element is removed from the
 *     chain, they may still point to elements in the chain or to invalid locations.
 */
template <class Elem, int INDEX = 0>
class DChain
{
  private:

    /**
     * %Chain iterator.
     */
    template <class IterElem>
    class ChainIterator : public IteratorBase<IterElem>
    {
      private:

        using IteratorBase<IterElem>::elem;

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        ChainIterator() :
          IteratorBase<IterElem>( nullptr )
        {}

        /**
         * Create chain iterator, initially pointing to the given element.
         */
        OZ_ALWAYS_INLINE
        explicit ChainIterator( IterElem* first ) :
          IteratorBase<IterElem>( first )
        {}

        /**
         * Advance to the next element.
         */
        OZ_ALWAYS_INLINE
        ChainIterator& operator ++ ()
        {
          hard_assert( elem != nullptr );

          elem = elem->next[INDEX];
          return *this;
        }

    };

  public:

    /**
     * %Iterator with constant access to container elements.
     */
    typedef ChainIterator<const Elem> CIterator;

    /**
     * %Iterator with non-constant access to container elements.
     */
    typedef ChainIterator<Elem> Iterator;

  private:

    Elem* firstElem; ///< Pointer to the first element in the chain.
    Elem* lastElem;  ///< Pointer to the last element in the chain.

  public:

    /**
     * Create an empty chain.
     */
    DChain() :
      firstElem( nullptr ), lastElem( nullptr )
    {}

    /**
     * Move constructor, rebinds elements to the new chain.
     */
    DChain( DChain&& c ) :
      firstElem( c.firstElem ), lastElem( c.lastElem )
    {
      c.firstElem = nullptr;
      c.lastElem  = nullptr;
    }

    /**
     * Move operator, rebinds elements to the destination chain.
     */
    DChain& operator = ( DChain&& c )
    {
      if( &c == this ) {
        return *this;
      }

      firstElem = c.firstElem;
      lastElem  = c.lastElem;

      c.firstElem = nullptr;
      c.lastElem  = nullptr;

      return *this;
    }

    /**
     * Create a copy of the chain and all its elements.
     */
    DChain clone() const
    {
      DChain clone;

      Elem* original = firstElem;
      Elem* prevCopy = nullptr;

      while( original != nullptr ) {
        Elem* copy = new Elem( *original );

        if( prevCopy == nullptr ) {
          clone.firstElem = copy;
        }
        else {
          prevCopy->next[INDEX] = copy;
        }
        copy->prev[INDEX] = prevCopy;

        original = original->next[INDEX];
        prevCopy = copy;
      }
      clone.lastElem = prevCopy;

      return clone;
    }

    /**
     * True iff same size and respective elements are equal.
     *
     * `Elem` type should implement `operator ==`, otherwise comparison doesn't make sense as two
     * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
     */
    bool equals( const DChain& c ) const
    {
      Elem* e1 = firstElem;
      Elem* e2 = c.firstElem;

      while( e1 != nullptr && e2 != nullptr && *e1 == *e2 ) {
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
      return CIterator( firstElem );
    }

    /**
     * %Iterator with non-constant access, initially points to the first element.
     */
    OZ_ALWAYS_INLINE
    Iterator iter() const
    {
      return Iterator( firstElem );
    }

    /**
     * STL-compatible constant begin iterator.
     */
    OZ_ALWAYS_INLINE
    CIterator begin() const
    {
      return CIterator( firstElem );
    }

    /**
     * STL-compatible begin iterator.
     */
    OZ_ALWAYS_INLINE
    Iterator begin()
    {
      return Iterator( firstElem );
    }

    /**
     * STL-compatible constant end iterator.
     */
    OZ_ALWAYS_INLINE
    CIterator end() const
    {
      return CIterator();
    }

    /**
     * STL-compatible end iterator.
     */
    OZ_ALWAYS_INLINE
    Iterator end()
    {
      return Iterator();
    }

    /**
     * Iterate through the chain and count elements.
     */
    int length() const
    {
      int i = 0;
      Elem* p = firstElem;

      while( p != nullptr ) {
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
      return firstElem == nullptr;
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
      hard_assert( e != nullptr );

      Elem* p = firstElem;

      while( p != nullptr && p != e ) {
        p = p->next[INDEX];
      }
      return p != nullptr;
    }

    /**
     * True iff an element equal to the given one is in the chain.
     *
     * `Elem` type should implement `operator ==`, otherwise comparison doesn't make sense as two
     * copies always differ in `prev[INDEX]` and `next[INDEX]` members.
     */
    bool contains( const Elem* e ) const
    {
      hard_assert( e != nullptr );

      Elem* p = firstElem;

      while( p != nullptr && !( *p == *e ) ) {
        p = p->next[INDEX];
      }
      return p != nullptr;
    }

    /**
     * Bind an element to the beginning of the chain.
     *
     * For efficiency reasons, elements are added to the beginning of a chain in contrast with array
     * chain.
     */
    void add( Elem* e )
    {
      pushFirst( e );
    }

    /**
     * Bind an element after some given element in the chain.
     */
    void insertAfter( Elem* e, Elem* p )
    {
      hard_assert( e != nullptr && p != nullptr );

      Elem* next = p->next[INDEX];

      e->prev[INDEX] = p;
      e->next[INDEX] = p->next[INDEX];
      p->next[INDEX] = e;

      if( next == nullptr ) {
        lastElem = e;
      }
      else {
        next->prev[INDEX] = e;
      }
    }

    /**
     * Bind an element before some given element in the chain.
     */
    void insertBefore( Elem* e, Elem* p )
    {
      hard_assert( e != nullptr && p != nullptr );

      Elem* prev = p->prev[INDEX];

      e->next[INDEX] = p;
      e->prev[INDEX] = prev;
      p->prev[INDEX] = e;

      if( prev == nullptr ) {
        firstElem = e;
      }
      else {
        p->prev[INDEX] = e;
      }
    }

    /**
     * Unbind the first element from the chain.
     *
     * To keep LIFO behaviour for `add()` and `erase()` methods like in array lists, the first
     * element is removed instead of the last one.
     */
    void erase()
    {
      popFirst();
    }

    /**
     * Unbind the given element from the chain.
     */
    void erase( Elem* e )
    {
      if( e->prev[INDEX] == nullptr ) {
        firstElem = e->next[INDEX];
      }
      else {
        e->prev[INDEX]->next[INDEX] = e->next[INDEX];
      }
      if( e->next[INDEX] == nullptr ) {
        lastElem = e->prev[INDEX];
      }
      else {
        e->next[INDEX]->prev[INDEX] = e->prev[INDEX];
      }
    }

    /**
     * Bind an element to the beginning of the chain.
     */
    void pushFirst( Elem* e )
    {
      hard_assert( e != nullptr );

      e->prev[INDEX] = nullptr;
      e->next[INDEX] = firstElem;

      if( firstElem == nullptr ) {
        firstElem = e;
        lastElem = e;
      }
      else {
        firstElem->prev[INDEX] = e;
        firstElem = e;
      }
    }

    /**
     * Bind an element to the end of the chain.
     */
    void pushLast( Elem* e )
    {
      hard_assert( e != nullptr );

      e->prev[INDEX] = lastElem;
      e->next[INDEX] = nullptr;

      if( lastElem == nullptr ) {
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
      hard_assert( firstElem != nullptr );

      Elem* e = firstElem;

      firstElem = firstElem->next[INDEX];

      if( firstElem == nullptr ) {
        lastElem = nullptr;
      }
      else {
        firstElem->prev[INDEX] = nullptr;
      }
      return e;
    }

    /**
     * Pop the last element from the chain.
     */
    Elem* popLast()
    {
      hard_assert( lastElem != nullptr );

      Elem* e = lastElem;

      lastElem = lastElem->prev[INDEX];

      if( lastElem == nullptr ) {
        firstElem = nullptr;
      }
      else {
        lastElem->next[INDEX] = nullptr;
      }
      return e;
    }

    /**
     * Empty the chain but do not delete the elements.
     */
    void clear()
    {
      firstElem = nullptr;
      lastElem = nullptr;
    }

    /**
     * Empty the chain and delete all elements.
     */
    void free()
    {
      Elem* p = firstElem;

      while( p != nullptr ) {
        Elem* next = p->next[INDEX];

        delete p;
        p = next;
      }

      firstElem = nullptr;
      lastElem = nullptr;
    }

};

}
