/*
 *  ReuseAlloc.h
 *
 *  Reuse allocator base class
 *  Base class for memory reusing. All classes which inherit from ReuseAlloc will have overloaded
 *  new and delete operators. The new operator will try to reuse memory from deleted objects form
 *  that class. At least the end of the program you should call Class::deallocate() for all
 *  ReuseAlloc-derived classes. Performance can increase up to 100%.
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  template <class Type, int INDEX = 0>
  struct ReuseAlloc
  {
    private:

      static Type* freeList;

    public:

      // If the list of freed blocks isn't empty, reuse the last freed block (at the beginning of
      // the list), otherwise allocate new block. (Constructor is called automatically.)
      void* operator new ( uint size )
      {
        if( freeList != null ) {
          ReuseAlloc* p = freeList;
          freeList = freeList->next[INDEX];

          return p;
        }
        return ::new byte[size];
      }

      // Do not really free memory, add it at the beginning of the list of freed blocks.
      // (Destructor is called automatically.)
      void operator delete ( void* ptr )
      {
        Type* object = reinterpret_cast<Type*>( ptr );

        // note that space for destroyed object is still allocated
        object->next[INDEX] = freeList;
        freeList = object;
      }

      // It's good idea to call that function from time to time and at the end of the program to
      // free some memory and to prevent memory leaks.
      static void deallocate()
      {
        Type* object = freeList;
        Type* nextObject;

        while( object != null ) {
          nextObject = object->next[INDEX];
          ::delete object;
          object = nextObject;
        }
        freeList = null;
      }

  };

  template <class Type, int INDEX>
  Type* ReuseAlloc<Type, INDEX>::freeList = null;

}
