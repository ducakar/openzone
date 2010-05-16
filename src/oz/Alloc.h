/*
 *  Alloc.h
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

/*
 * Standard new and delete declarations (may be omitted as already built into C++ language)
 */
void* operator new ( oz::uint size );
void* operator new[] ( oz::uint size );
void operator delete ( void* ptr );
void operator delete[] ( void* ptr );

/*
 * Placement new and delete
 */
inline void* operator new ( oz::uint, void* place ) throw() { return place; }
inline void* operator new[] ( oz::uint, void* place ) throw() { return place; }
inline void operator delete ( void*, void* ) throw() {}
inline void operator delete[] ( void*, void* ) throw() {}

namespace oz
{

  class Alloc
  {
    public:

      static int  count;
      static long amount;

      static int  sumCount;
      static long sumAmount;

      static int  maxCount;
      static long maxAmount;

      /**
       * Allocate memory without constructing the elements. Memory has to be freed via
       * Alloc::deallocate.
       * @param size
       * @return
       */
      template <typename Type>
      static Type* allocate( int size )
      {
        return reinterpret_cast<Type*>( new char[size * sizeof( Type )] );
      }

      /**
       * Free memory allocated with Alloc::allocate function without destructing the elements.
       * @param ptr
       */
      template <typename Type>
      static void deallocate( Type* ptr )
      {
        delete[] reinterpret_cast<char*>( ptr );
      }

      /**
       * Allocates new memory for the array for newSize elements to fit in. Elements are constructed
       * with copy constructor in the new copy of the array and destructed in the old copy before it
       * is freed. The memory has to be allocated with Alloc::allocate function and freed with
       * Alloc::deallocate.
       * @param array
       * @param count number of elements from the beginning of the array that are constructed
       * @param newSize new size of the array
       * @return
       */
      template <typename Type>
      static Type* reallocate( Type* array, int count, int newSize )
      {
        Type* newArray = reinterpret_cast<Type*>( new char[newSize * sizeof( Type )] );
        for( int i = 0; i < count; ++i ) {
          new( newArray + i ) Type( array[i] );
          array[i].~Type();
        }
        delete[] reinterpret_cast<char*>( array );
        return newArray;
      }

  };

}
