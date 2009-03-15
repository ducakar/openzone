/*
 *  HashString.hpp
 *
 *  Chaining hashtable implementation with String key type
 *  A prime number is recommended as hashtable size.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type, int SIZE>
  class HashString
  {
    private:

      struct Elem : PoolAlloc<Elem, 0>
      {
        String key;
        Type   value;
        Elem   *next[1];

        Elem()
        {}

        Elem( const String &key_, const Type &value_, Elem *next_ ) : key( key_ ), value( value_ )
        {
          next[0] = next_;
        }

        Elem( const String &key_, Elem *next_ ) : key( key_ )
        {
          next[0] = next_;
        }
      };

    public:

      /**
       * HashString iterator.
       */
      class Iterator : public IteratorBase<Elem>
      {
        private:

          // base class
          typedef IteratorBase<Elem> B;

          Elem **data;
          int  index;

        public:

          /**
           * Copy constructor.
           * @param t
           */
          explicit Iterator( HashString &t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              index++;
              B::elem = data[index];
            }
          }

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
           * Advance to the next element.
           * @param
           */
          void operator ++ ( int )
          {
            assert( B::elem != null );

            if( B::elem->next[0] != null ) {
              B::elem = B::elem->next[0];
            }
            else if( index < SIZE - 1 ) {
              do {
                index++;
                B::elem = data[index];
              }
              while( B::elem == null && index < SIZE - 1 );
            }
            else {
              B::elem = null;
            }
          }

          /**
           * @return current element's key
           */
          String *key() const
          {
            return &B::elem->key;
          }

          /**
           * @return pointer to current element's value
           */
          Type *value()
          {
            return &B::elem->value;
          }

          /**
           * @return constant pointer to current element's value
           */
          const Type *value() const
          {
            return &B::elem->value;
          }

          /**
           * @return pointer to current element
           */
          operator Type* ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant pointer to current element
           */
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return reference to current element
           */
          Type &operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Type &operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return non-constant access to member
           */
          Type *operator -> ()
          {
            return &B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Type *operator -> () const
          {
            return &B::elem->value;
          }

      };

    private:

      Elem *data[SIZE];
      // we cache found element since we often want its value after a search
      Elem *cached;
      int  count;

      /**
       * @param chainA
       * @param chainB
       * @return true if chains are equal length and all elements are equal
       */
      bool areChainsEqual( const Elem *chainA, const Elem *chainB ) const
      {
        if( chainA != null && chainB != null ) {
          return
              chainA->key == chainB->key &&
              chainA->value == chainB->value &&
              areChainsEqual( chainA->next[0], chainB->next[0] );
        }
        else {
          // at least one is null, so (chainA == chainB) <=> (chainA == null && chainB == null)
          return chainA == chainB;
        }
      }

      /**
       * Allocate space and make a copy of a given chain.
       * @param chain
       * @return pointer to first element of newly allocated chain
       */
      Elem *copyChain( const Elem *chain ) const
      {
        if( chain != null ) {
          return new Elem( chain->key, chain->value, copyChain( chain->next[0] ) );
        }
        else {
          return null;
        }
      }

      /**
       * Delete all elements in given chain.
       * @param chain
       */
      void freeChain( const Elem *chain ) const
      {
        if( chain != null ) {
          freeChain( chain->next[0] );

          delete chain;
        }
      }

      /**
       * Delete all elements and their values in given chain.
       * @param chain
       */
      void freeChainAndValues( const Elem *chain ) const
      {
        if( chain != null ) {
          freeChain( chain->next[0] );

          delete chain->value;
          delete chain;
        }
      }

    public:

      /**
       * Constructor.
       */
      HashString() : cached( null ), count( 0 )
      {
        for( int i = 0; i < SIZE; i++ ) {
          data[i] = null;
        }
      }

      /**
       * Copy constructor.
       * @param t
       */
      HashString( const HashString &t ) : cached( t.cached ), count( t.count )
      {
        for( int i = 0; i < SIZE; i++ ) {
          data[i] = copyChain( t.data[i] );
        }
      }

      /**
       * Default destructor.
       */
      ~HashString()
      {
        assert( count == 0 );
      }

      /**
       * Copy operator.
       * @param t
       * @return
       */
      HashString &operator = ( const HashString &t )
      {
        for( int i = 0; i < SIZE; i++ ) {
          if( data[i] != null ) {
            delete data[i];
          }
          data[i] = copyChain( t.data[i] );
        }
        cached = t.cached;
        count = t.count;
      }

      /**
       * Equality operator.
       * @param t
       * @return
       */
      bool operator == ( const HashString &t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; i++ ) {
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
      bool operator != ( const HashString &t ) const
      {
        if( count != t.count ) {
          return false;
        }
        for( int i = 0; i < SIZE; i++ ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @return iterator for this HashString
       */
      Iterator iterator()
      {
        return Iterator( *this );
      }

      /**
       * @return number of elements
       */
      int length() const
      {
        return count;
      }

      /**
       * @return capacity
       */
      int capacity() const
      {
        return SIZE;
      }

      /**
       * @return true if HashString has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * @return cached element's key
       */
      uint &cachedKey() const
      {
        return cached->key;
      }

      /**
       * @return cached element's value
       */
      Type &cachedValue() const
      {
        return cached->value;
      }

      /**
       * Find element with given value.
       * This function caches the found element.
       * @param key
       * @return true if found
       */
      bool contains( const String &key )
      {
        int  i  = key.hash() % SIZE;
        Elem *p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            cached = p;
            return true;
          }
          else {
            p = p->next[0];
          }
        }
        return false;
      }

      /**
       * If given key exists, return reference to it's value, otherwise create a new element with
       * the given key and return reference to it's UNINITIALIZED value.
       * This function caches the found element.
       * @param key
       * @return reference to value associated to the given key
       */
      Type &operator [] ( const String &key )
      {
        if( !contains( key ) ) {
          int i = key.hash() % SIZE;
          Elem *elem = new Elem( key, data[i] );

          data[i] = elem;
          cached = elem;
          count++;
        }
        return cached->value;
      }

      /**
       * Add new element. The key must not yet exist in this HashIndex.
       * @param key
       * @param value
       */
      void add( const String &key, const Type &value )
      {
        assert( !contains( key ) );

        int i = key.hash() % SIZE;

        Elem *elem = new Elem( key, value, data[i] );

        data[i] = elem;
        cached = elem;
        count++;
      }

      /**
       * Remove element with given key.
       * @param key
       */
      void remove( const String &key )
      {
        int  i  = key.hash() % SIZE;
        Elem *p = data[i];
        Elem **prev = &data[i];

        while( p != null ) {
          if( p->key == key ) {
            *prev = p->next[0];
            delete p;
            count--;
            return;
          }
          else {
            prev = &p->next[0];
            p = p->next[0];
          }
        }
      }

      /**
       * Remove all elements.
       */
      void clear()
      {
        for( int i = 0; i < SIZE; i++ ) {
          freeChain( data[i] );
          data[i] = null;
        }
        cached = null;
        count = 0;
      }

      /**
       * Remove all elements and delete their values.
       */
      void free()
      {
        for( int i = 0; i < SIZE; i++ ) {
          freeChainAndValues( data[i] );
          data[i] = null;
        }
        cached = null;
        count = 0;
      }

      /**
       * Deallocate memory from PoolAlloc.
       */
      static void deallocate()
      {
        PoolAlloc<Elem, 0>::pool.free();
      }

  };

}
