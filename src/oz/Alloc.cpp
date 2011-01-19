/*
 *  Alloc.cpp
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "Alloc.hpp"

#include <cstdlib>

namespace oz
{

  static_assert( ( Alloc::ALIGNMENT & ( Alloc::ALIGNMENT - 1 ) ) == 0,
                 "Alloc::ALIGNMENT should be power of two" );

  int  Alloc::count     = 0;
  long Alloc::amount    = 0;

  int  Alloc::sumCount  = 0;
  long Alloc::sumAmount = 0;

  int  Alloc::maxCount  = 0;
  long Alloc::maxAmount = 0;

}

using oz::max;
using oz::Alloc;

#ifndef OZ_ALLOC_STATISTICS

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

  return ptr;
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

  return ptr;
}

void operator delete ( void* ptr ) throw()
{
  free( ptr );
}

void operator delete[] ( void* ptr ) throw()
{
  free( ptr );
}

#else

static_assert( sizeof( size_t ) < size_t( Alloc::ALIGNMENT ),
               "Alloc::ALIGNEMENT should not be less than sizeof( size_t ) when using memory "
               "allocation statistics." );

#ifdef OZ_MSVC
void* operator new ( size_t size )
#else
void* operator new ( size_t size ) throw( std::bad_alloc )
#endif
{
  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  reinterpret_cast<size_t*>( ptr )[0] = size;
  return reinterpret_cast<char*>( ptr ) + Alloc::ALIGNMENT;
}

#ifdef OZ_MSVC
void* operator new[] ( size_t size )
#else
void* operator new[] ( size_t size ) throw( std::bad_alloc )
#endif
{
  size += Alloc::ALIGNMENT;

  void* ptr;
  if( posix_memalign( &ptr, Alloc::ALIGNMENT, size ) ) {
    throw std::bad_alloc();
  }

  ++Alloc::count;
  Alloc::amount += size;

  ++Alloc::sumCount;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  reinterpret_cast<size_t*>( ptr )[0] = size;
  return reinterpret_cast<char*>( ptr ) + Alloc::ALIGNMENT;
}

void operator delete ( void* ptr ) throw()
{
  assert( ptr != null );

  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;
  size_t size  = reinterpret_cast<size_t*>( chunk )[0];

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

void operator delete[] ( void* ptr ) throw()
{
  assert( ptr != null );

  char*  chunk = reinterpret_cast<char*>( ptr ) - Alloc::ALIGNMENT;
  size_t size  = reinterpret_cast<size_t*>( chunk )[0];

  --Alloc::count;
  Alloc::amount -= size;

  free( chunk );
}

#endif
