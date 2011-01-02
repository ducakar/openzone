/*
 *  Alloc.hpp
 *
 *  Overload default new and delete operators for slightly better performance or provide heap
 *  allocation statistics (if defined OZ_ALLOC_STATISTICS).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

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
       * <code>Alloc::dealloc</code>.
       * @param size
       * @return
       */
      template <typename Type>
      static Type* alloc( int size )
      {
        return reinterpret_cast<Type*>( new char[ size * sizeof( Type ) ] );
      }

      /**
       * Free memory allocated with <code>Alloc::alloc</code> function without destructing the
       * elements.
       * @param ptr
       */
      template <typename Type>
      static void dealloc( Type* ptr )
      {
        delete[] reinterpret_cast<char*>( ptr );
      }

      /**
       * Allocates new memory for the array for newSize elements to fit in. Elements are constructed
       * via move constructor in the new copy of the array and destructed in the old copy before it
       * is freed. The memory has to be allocated with <code>Alloc::alloc</code> function and freed
       * with <code>Alloc::dealloc</code>.
       * It's similar to aRealloc, but works on arrays where only first <code>count</code> elements
       * are constructed.
       * @param array
       * @param count number of elements from the beginning of the array that are constructed
       * @param newSize new size of the array
       * @return
       */
      template <typename Type>
      static Type* realloc( Type* array, int count, int newSize )
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
