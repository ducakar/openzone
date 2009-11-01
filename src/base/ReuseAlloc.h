/*
 *  ReuseAlloc.h
 *
 *  Reuse allocator
 *  Base class for memory reusing. All classes which inherit from ReuseAlloc will have overloaded
 *  new and delete (BUT NOT new[] and delete[]) operators. The new operator will try to reuse
 *  memory from deleted objects form that class. At least the end of the program you should call
 *  Class::deallocate() for all Reuser-derived classes. Performance can increase up to 100%.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  template <class Type>
  class ReuseAlloc
  {
    private:

      static Type *freeList;

      Type *next;

      void free() const
      {
        if( next != null ) {
          next->ReuseAlloc::free();
        }
        ::delete this;
      }

    public:

      ReuseAlloc() : next( null )
      {}

#ifdef OZ_REUSEALLOC
      // If the list of freed blocks isn't empty, reuse the last freed block (at the beginning of
      // the list), otherwise allocate new block. (Constructor is called automatically.)
      void *operator new ( uint size )
      {
        if( freeList != null ) {
          ReuseAlloc *p = freeList;
          freeList = freeList->ReuseAlloc::next;

          return p;
        }
        return ::new byte[size];
      }

      /**
       * No placement new.
       */
      void *operator new ( uint, void* )
      {
        throw Exception( "No placement new" );
      }

      // Do not really free memory, add it at the beginning of the list of freed blocks.
      // (Destructor is called automatically.)
      void operator delete ( void *ptr )
      {
        Type *p = reinterpret_cast<Type*>( ptr );

        // note that space for destroyed object is still allocated
        p->ReuseAlloc::next = freeList;
        freeList = p;
      }

      /**
       * No placement delete
       */
      void operator delete ( void*, void * )
      {
        throw Exception( "No placement delete" );
      }
#endif

      // It's good idea to call that function from time to time and at the end of the program to
      // free some memory and to prevent memory leaks.
      static void deallocate()
      {
        if( freeList != null ) {
          freeList->ReuseAlloc::free();
          freeList = null;
        }
      }

  };

  template <class Type>
  Type *ReuseAlloc<Type>::freeList = null;

}
