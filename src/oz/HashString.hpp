/*
 *  HashString.hpp
 *
 *  Chaining hashtable implementation with String key type
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "String.hpp"
#include "Pool.hpp"

namespace oz
{

  template <typename Type = nullptr_t, int SIZE = 256>
  class HashString
  {
    static_assert( SIZE > 0, "HashString size must be at least 1" );

    private:

      struct Elem
      {
        Elem*        next;
        const String key;
        Type         value;

        OZ_ALWAYS_INLINE
        explicit Elem( Elem* next_, const char* key_, const Type& value_ ) :
            next( next_ ), key( key_ ), value( value_ )
        {}

        OZ_PLACEMENT_POOL_ALLOC( Elem, SIZE )
      };

    public:

      /**
       * Constant HashString iterator.
       */
      class CIterator : public CIteratorBase<Elem>
      {
        private:

          typedef CIteratorBase<Elem> B;

          Elem* const* data;
          int index;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          CIterator() : B( null )
          {}

          /**
           * Make iterator for given HashString. After creation it points to first element.
           * @param t
           */
          explicit CIterator( const HashString& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * @return constant pointer to current element
           */
          OZ_ALWAYS_INLINE
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          OZ_ALWAYS_INLINE
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          OZ_ALWAYS_INLINE
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return current element's key
           */
          OZ_ALWAYS_INLINE
          const String& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * Advance to the next element.
           * @return
           */
          CIterator& operator ++ ()
          {
            hard_assert( B::elem != null );

            if( B::elem->next != null ) {
              B::elem = B::elem->next;
            }
            else if( index < SIZE - 1 ) {
              do {
                ++index;
                B::elem = data[index];
              }
              while( B::elem == null && index < SIZE - 1 );
            }
            else {
              B::elem = null;
            }
            return *this;
          }

      };

      /**
       * HashString iterator.
       */
      class Iterator : public IteratorBase<Elem>
      {
        private:

          typedef IteratorBase<Elem> B;

          Elem* const* data;
          int index;

        public:

          /**
           * Default constructor returns an invalid iterator
           */
          OZ_ALWAYS_INLINE
          Iterator() : B( null )
          {}

          /**
           * Make iterator for given HashString. After creation it points to first element.
           * @param t
           */
          explicit Iterator( const HashString& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * @return constant pointer to current element
           */
          OZ_ALWAYS_INLINE
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return pointer to current element
           */
          OZ_ALWAYS_INLINE
          operator Type* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          OZ_ALWAYS_INLINE
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element
           */
          OZ_ALWAYS_INLINE
          Type& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          OZ_ALWAYS_INLINE
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return non-constant access to member
           */
          OZ_ALWAYS_INLINE
          Type* operator -> ()
          {
            return &B::elem->value;
          }

          /**
           * @return current element's key
           */
          OZ_ALWAYS_INLINE
          const String& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          OZ_ALWAYS_INLINE
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
          OZ_ALWAYS_INLINE
          Type& value()
          {
            return B::elem->value;
          }

          /**
           * Advance to the next element.
           * @return
           */
          Iterator& operator ++ ()
          {
            hard_assert( B::elem != null );

            if( B::elem->next != null ) {
              B::elem = B::elem->next;
            }
            else if( index < SIZE - 1 ) {
              do {
                ++index;
                B::elem = data[index];
              }
              while( B::elem == null && index < SIZE - 1 );
            }
            else {
              B::elem = null;
            }
            return *this;
          }

      };

    private:

      Elem*            data[SIZE];
      Pool<Elem, SIZE> pool;
      int              count;

      /**
       * @param chainA
       * @param chainB
       * @return true if chains are equal length and all elements are equal
       */
      static bool areChainsEqual( const Elem* chainA, const Elem* chainB )
      {
        while( chainA != null && chainB != null ) {
          if( chainA->key != chainB->key || chainA->value != chainB->value ) {
            return false;
          }

          chainA = chainA->next;
          chainB = chainB->next;
        }
        // at least one is null, so (chainA == chainB) <=> (chainA == null && chainB == null)
        return chainA == chainB;
      }

      /**
       * Allocate space and make a copy of a given chain.
       * @param chain
       * @return pointer to first element of newly allocated chain
       */
      Elem* copyChain( const Elem* chain )
      {
        Elem* newChain = null;

        while( chain != null ) {
          newChain = new( pool ) Elem( newChain, chain->key, chain->value );
          chain = chain->next;
        }
        return newChain;
      }

      /**
       * Delete all elements in given chain.
       * @param chain
       */
      void freeChain( Elem* chain )
      {
        while( chain != null ) {
          Elem* next = chain->next;

          chain->~Elem();
          pool.dealloc( chain );

          chain = next;
        }
      }

      /**
       * Delete all elements and their values in given chain.
       * @param chain
       */
      void freeChainAndValues( Elem* chain )
      {
        while( chain != null ) {
          Elem* next = chain->next;

          delete chain->value;
          chain->~Elem();
          pool.dealloc( chain );

          chain = next;
        }
      }

    public:

      /**
       * Constructor.
       */
      HashString() : count( 0 )
      {
        aSet<Elem*>( data, null, SIZE );
      }

      /**
       * Destructor.
       */
      ~HashString()
      {
        clear();
        dealloc();
      }

      /**
       * Copy constructor.
       * @param t
       */
      HashString( const HashString& t ) : count( t.count )
      {
        for( int i = 0; i < SIZE; ++i ) {
          data[i] = copyChain( t.data[i] );
        }
      }

      /**
       * Copy operator.
       * @param t
       * @return
       */
      HashString& operator = ( const HashString& t )
      {
        if( &t == this ) {
          soft_assert( &t != this );
          return *this;
        }

        for( int i = 0; i < SIZE; ++i ) {
          freeChain( data[i] );
          data[i] = copyChain( t.data[i] );
        }
        count = t.count;

        return *this;
      }

      /**
       * Equality operator.
       * @param t
       * @return
       */
      bool operator == ( const HashString& t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; ++i ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return false;
          }
        }
        return true;
      }

      /**
       * Inequality operator.
       * @param t
       * @return
       */
      bool operator != ( const HashString& t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; ++i ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @return constant iterator for this HashString
       */
      OZ_ALWAYS_INLINE
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this HashString
       */
      OZ_ALWAYS_INLINE
      Iterator iter() const
      {
        return Iterator( *this );
      }

      /**
       * @return number of elements
       */
      OZ_ALWAYS_INLINE
      int length() const
      {
        return count;
      }

      /**
       * @return true if HashString has no elements
       */
      OZ_ALWAYS_INLINE
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @return capacity
       */
      OZ_ALWAYS_INLINE
      int capacity() const
      {
        return SIZE;
      }

      /**
       * @return load factor of hashtable (number of elements / capacity)
       */
      float loadFactor() const
      {
        return float( count ) / float( SIZE );
      }

      /**
       * Find element with given value.
       * @param key
       * @return true if found
       */
      bool contains( const char* key ) const
      {
        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return true;
          }
          else {
            p = p->next;
          }
        }
        return false;
      }

      /**
       * If given key exists, return constant pointer to its value, otherwise return null.
       * @param key
       * @return
       */
      const Type* find( const char* key ) const
      {
        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return &p->value;
          }
          else {
            p = p->next;
          }
        }
        return null;
      }

      /**
       * If given key exists, return pointer to its value, otherwise return null.
       * @param key
       * @return
       */
      Type* find( const char* key )
      {
        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return &p->value;
          }
          else {
            p = p->next;
          }
        }
        return null;
      }

      /**
       * If given key exists, return constant reference to its value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return constant reference to value associated to the given key
       */
      const Type& get( const char* key ) const
      {
        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return p->value;
          }
          else {
            p = p->next;
          }
        }

        hard_assert( false );

        return data[0]->value;
      }

      /**
       * If given key exists, return reference to its value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      Type& get( const char* key )
      {
        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* p = data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            return p->value;
          }
          else {
            p = p->next;
          }
        }

        hard_assert( false );

        return data[0]->value;
      }

      /**
       * Add new element. The key must not yet exist in this HashString.
       * @param key
       * @param value
       * @return pointer to new entry's value
       */
      Type* add( const char* key, const Type& value = Type() )
      {
        hard_assert( !contains( key ) );

        uint  i = uint( String::hash( key ) ) % uint( SIZE );
        Elem* elem = new( pool ) Elem( data[i], key, value );

        data[i] = elem;
        ++count;

        soft_assert( loadFactor() < 0.75f );

        return &elem->value;
      }

      /**
       * Remove element with given key.
       * @param key
       */
      void exclude( const char* key )
      {
        uint   i = uint( String::hash( key ) ) % uint( SIZE );
        Elem*  p = data[i];
        Elem** prev = &data[i];

        while( p != null ) {
          if( p->key.equals( key ) ) {
            *prev = p->next;
            --count;

            p->~Elem();
            pool.dealloc( p );
            return;
          }
          else {
            prev = &p->next;
            p = p->next;
          }
        }

        hard_assert( false );
      }

      /**
       * Remove all elements.
       */
      void clear()
      {
        for( int i = 0; i < SIZE; ++i ) {
          freeChain( data[i] );
          data[i] = null;
        }
        count = 0;
      }

      /**
       * Remove all elements and delete their values. Only if values are pointers.
       */
      void free()
      {
        for( int i = 0; i < SIZE; ++i ) {
          freeChainAndValues( data[i] );
          data[i] = null;
        }

        count = 0;
        pool.free();
      }

      /**
       * Deallocate memory from Pool.
       */
      void dealloc()
      {
        hard_assert( count == 0 );

        pool.free();
      }

  };

}
