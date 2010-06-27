/*
 *  HashIndex.hh
 *
 *  Chaining hashtable implementation with uint key type.
 *  A prime number is recommended as hashtable size unless key distribution is "random".
 *  You can find a list of millions of primes at http://www.bigprimes.net/.
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  template <typename Type = nil, int SIZE = 253>
  class HashIndex
  {
    private:

      struct Elem
      {
        uint  key;
        Type  value;
        Elem* next[1];

        explicit Elem( uint key_, const Type& value_, Elem* next_ ) : key( key_ ), value( value_ )
        {
          next[0] = next_;
        }

        explicit Elem( uint key_, Elem* next_ ) : key( key_ )
        {
          next[0] = next_;
        }

        OZ_PLACEMENT_POOL_ALLOC( Elem, 0, SIZE )
      };

    public:

      /**
       * Constant HashIndex iterator.
       */
      class CIterator : public CIteratorBase<Elem>
      {
        private:

          typedef CIteratorBase<Elem> B;

          const Elem* const* const data;
          int index;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit CIterator() : B( null )
          {}

          /**
           * Make iterator for given HashIndex. After creation it points to first element.
           * @param t
           */
          explicit CIterator( const HashIndex& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * Advance to the next element.
           * @param
           */
          CIterator& operator ++ ()
          {
            assert( B::elem != null );

            if( B::elem->next[0] != null ) {
              B::elem = B::elem->next[0];
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

          /**
           * @return current element's key
           */
          const uint& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * @return constant pointer to current element
           */
          operator const Type* () const
          {
            return &B::elem->value;
          }

          /**
           * @return constant reference to current element
           */
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

      };

      /**
       * HashIndex iterator.
       */
      class Iterator : public IteratorBase<Elem>
      {
        private:

          typedef IteratorBase<Elem> B;

          Elem* const* const data;
          int index;

        public:

          /**
           * Default constructor returns a dummy passed iterator
           * @return
           */
          explicit Iterator() : B( null )
          {}

          /**
           * Make iterator for given HashIndex. After creation it points to first element.
           * @param t
           */
          explicit Iterator( const HashIndex& t ) : B( t.data[0] ), data( t.data ), index( 0 )
          {
            while( B::elem == null && index < SIZE - 1 ) {
              ++index;
              B::elem = data[index];
            }
          }

          /**
           * Advance to the next element.
           * @param
           */
          Iterator& operator ++ ()
          {
            assert( B::elem != null );

            if( B::elem->next[0] != null ) {
              B::elem = B::elem->next[0];
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

          /**
           * @return current element's key
           */
          const uint& key() const
          {
            return B::elem->key;
          }

          /**
           * @return constant reference to current element's value
           */
          const Type& value() const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element's value
           */
          Type& value()
          {
            return B::elem->value;
          }

          /**
           * @return constant pointer to current element
           */
          operator const Type* () const
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
           * @return constant reference to current element
           */
          const Type& operator * () const
          {
            return B::elem->value;
          }

          /**
           * @return reference to current element
           */
          Type& operator * ()
          {
            return B::elem->value;
          }

          /**
           * @return constant access to member
           */
          const Type* operator -> () const
          {
            return &B::elem->value;
          }

          /**
           * @return non-constant access to member
           */
          Type* operator -> ()
          {
            return &B::elem->value;
          }

      };

    private:

      Pool<Elem, 0, SIZE> pool;
      Elem*               data[SIZE];
      int                 count;

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
          chainA = chainA->next[0];
          chainB = chainB->next[0];
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
          newChain = new( pool ) Elem( chain->key, chain->value, newChain );
          chain = chain->next[0];
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
          Elem* next = chain->next[0];

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
          Elem* next = chain->next[0];

          delete chain->value;
          pool.dealloc( chain );
          chain = next;
        }
      }

    public:

      /**
       * Constructor.
       */
      explicit HashIndex() : count( 0 )
      {
        for( int i = 0; i < SIZE; ++i ) {
          data[i] = null;
        }
      }

      /**
       * Copy constructor.
       * @param t
       */
      HashIndex( const HashIndex& t ) : count( t.count )
      {
        for( int i = 0; i < SIZE; ++i ) {
          data[i] = copyChain( t.data[i] );
        }
      }

      /**
       * Move constructor.
       * @param t
       */
      HashIndex( HashIndex&& t ) : pool( static_cast< Pool<Elem, 0, SIZE>&& >( t.pool ) ),
          count( t.count )
      {
        aCopy( data, t.data, SIZE );

        t.count = 0;
      }

      /**
       * Default destructor.
       */
      ~HashIndex()
      {
        assert( count == 0 );

        pool.free();
      }

      /**
       * Copy operator.
       * @param t
       * @return
       */
      HashIndex& operator = ( const HashIndex& t )
      {
        assert( &t != this );
        assert( count == 0 );

        for( int i = 0; i < SIZE; ++i ) {
          data[i] = copyChain( t.data[i] );
        }
        count = t.count;
        return *this;
      }

      /**
       * Move operator.
       * @param t
       * @return
       */
      HashIndex& operator = ( HashIndex&& t )
      {
        assert( &t != this );
        assert( count == 0 );

        aCopy( data, t.data, SIZE );
        pool = static_cast< Pool<Elem, 0, SIZE>&& >( t.pool );
        count = t.count;

        t.count = 0;
        return *this;
      }

      /**
       * Equality operator.
       * @param t
       * @return
       */
      bool operator == ( const HashIndex& t ) const
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
      bool operator != ( const HashIndex& t ) const
      {
        if( count != t.count ) {
          return true;
        }
        for( int i = 0; i < SIZE; ++i ) {
          if( !areChainsEqual( data[i], t.data[i] ) ) {
            return true;
          }
        }
        return false;
      }

      /**
       * @return constant iterator for this HashIndex
       */
      CIterator citer() const
      {
        return CIterator( *this );
      }

      /**
       * @return iterator for this HashIndex
       */
      Iterator iter() const
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
       * @return load factor of hashtable (number of elements / capacity)
       */
      float loadFactor() const
      {
        return float( count ) / float( SIZE );
      }

      /**
       * @return true if HashIndex has no elements
       */
      bool isEmpty() const
      {
        return count == 0;
      }

      /**
       * Find element with given value.
       * @param key
       * @return true if found
       */
      bool contains( uint key ) const
      {
        int   i = key % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            return true;
          }
          else {
            p = p->next[0];
          }
        }
        return false;
      }

      /**
       * Find element with given value
       * @param key
       * @return constant pointer to value of given key
       */
      const Type* find( uint key ) const
      {
        int   i = key % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            return &p->value;
          }
          else {
            p = p->next[0];
          }
        }
        return null;
      }

      /**
       * Find element with given value
       * @param key
       * @return pointer to value of given key
       */
      Type* find( uint key )
      {
        int   i = key % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            return &p->value;
          }
          else {
            p = p->next[0];
          }
        }
        return null;
      }

      /**
       * If given key exists, return constant reference to it's value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      const Type& operator [] ( uint key ) const
      {
        int   i = key % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            return p->value;
          }
          else {
            p = p->next[0];
          }
        }

        assert( false );

        return data[0].value;
      }

      /**
       * If given key exists, return reference to it's value.
       * Only use this function if you are certain that the key exists.
       * @param key
       * @return reference to value associated to the given key
       */
      Type& operator [] ( uint key )
      {
        int   i = key % SIZE;
        Elem* p = data[i];

        while( p != null ) {
          if( p->key == key ) {
            return p->value;
          }
          else {
            p = p->next[0];
          }
        }

        assert( false );

        return data[0].value;
      }

      /**
       * Add new element. The key must not yet exist in this HashIndex.
       * @param key
       * @param value
       */
      Type* add( uint key, const Type& value = Type() )
      {
        assert( !contains( key ) );

        int  i = key % SIZE;
        Elem* elem = new( pool ) Elem( key, value, data[i] );

        data[i] = elem;
        ++count;

        assert( loadFactor() < 0.75f );

        return &data[i]->value;
      }

      /**
       * Remove element with given key.
       * @param key
       */
      void remove( uint key )
      {
        int    i = key % SIZE;
        Elem*  p = data[i];
        Elem** prev = &data[i];

        while( p != null ) {
          if( p->key == key ) {
            *prev = p->next[0];
            pool.dealloc( p );
            --count;
            return;
          }
          else {
            prev = &p->next[0];
            p = p->next[0];
          }
        }

        assert( false );
      }

      /**
       * Remove all elements.
       */
      void clear()
      {
        for( int i = 0; i < SIZE; ++i ) {
          if( data[i] != null ) {
            freeChain( data[i] );
            data[i] = null;
          }
        }
        count = 0;
      }

      /**
       * Remove all elements and delete their values. Only if values are pointers.
       */
      void free()
      {
        for( int i = 0; i < SIZE; ++i ) {
          if( data[i] != null ) {
            freeChainAndValues( data[i] );
            data[i] = null;
          }
        }
        count = 0;
      }

      /**
       * Deallocate memory from Pool.
       */
      void deallocate()
      {
        assert( count == 0 );

        pool.free();
      }

  };

}
