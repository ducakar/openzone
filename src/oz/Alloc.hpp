/*
 *  Alloc.hpp
 *
 *  Overload default new and delete operators to provide allocation statistics and optionally
 *  check for leaks and mismatched new/delete (if OZ_TRACE_LEAKS is turned on).
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "common.hpp"

namespace oz
{

  class Alloc
  {
    public:

#ifdef OZ_SIMD
      static const size_t ALIGNMENT            = 16;
#else
      static const size_t ALIGNMENT            = sizeof( size_t );
#endif
      static const int    BACKTRACE_SIZE       = 16;
      static const int    DEMANGLE_BUFFER_SIZE = 1024;

      static int    count;
      static size_t amount;

      static int    sumCount;
      static size_t sumAmount;

      static int    maxCount;
      static size_t maxAmount;

      OZ_WEAK_SYMBOL
      static bool isLocked;

    private:

      // singleton
      Alloc();

    public:

      OZ_ALWAYS_INLINE
      static size_t alignDown( size_t size )
      {
        return size & ~( ALIGNMENT - 1 );
      }

      OZ_ALWAYS_INLINE
      static size_t alignUp( size_t size )
      {
        return ( ( size - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT;
      }

      template <typename Pointer>
      OZ_ALWAYS_INLINE
      static Pointer* alignDown( Pointer* p )
      {
        return reinterpret_cast<Pointer*>( size_t( p ) & ~( ALIGNMENT - 1 ) );
      }

      template <typename Pointer>
      OZ_ALWAYS_INLINE
      static Pointer* alignUp( Pointer* p )
      {
        return reinterpret_cast<Pointer*>( ( size_t( p - 1 ) & ~( ALIGNMENT - 1 ) ) + ALIGNMENT );
      }

      static void printStatistics();
      static void printLeaks();

  };

}
