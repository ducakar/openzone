/*
 *  Alloc.hpp
 *
 *  Overload default new and delete operators for slightly better performance
 *  (ifndef OZ_ALLOC_STATISTICS) or provide heap allocation statistics (ifdef OZ_ALLOC_STATISTICS).
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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
       * Alloc::dealloc.
       * @param size
       * @return
       */
      template <typename Type>
      static Type* alloc( int size )
      {
        return reinterpret_cast<Type*>( new char[ size * sizeof( Type ) ] );
      }

      /**
       * Free memory allocated with Alloc::alloc function without destructing the elements.
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
       * is freed. The memory has to be allocated with Alloc::alloc function and freed with
       * Alloc::dealloc.
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
