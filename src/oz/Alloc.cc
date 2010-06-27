/*
 *  Alloc.cc
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "oz.hh"

#include <cstdlib>

namespace oz
{

  int  Alloc::count     = 0;
  long Alloc::amount    = 0;

  int  Alloc::sumCount  = 0;
  long Alloc::sumAmount = 0;

  int  Alloc::maxCount  = 0;
  long Alloc::maxAmount = 0;

}

#ifndef OZ_ALLOC_STATISTICS

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  return malloc( size );
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  return malloc( size );
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

void* operator new ( size_t size ) throw( std::bad_alloc )
{
  size_t* p = reinterpret_cast<size_t*>( malloc( size + sizeof( size_t ) ) );

  if( p == null ) {
    throw std::bad_alloc();
  }

  ++oz::Alloc::count;
  oz::Alloc::amount += size;

  ++oz::Alloc::sumCount;
  oz::Alloc::sumAmount += size;

  oz::Alloc::maxCount = oz::max( oz::Alloc::count, oz::Alloc::maxCount );
  oz::Alloc::maxAmount = oz::max( oz::Alloc::amount, oz::Alloc::maxAmount );

  p[0] = size;
  return p + 1;
}

void* operator new[] ( size_t size ) throw( std::bad_alloc )
{
  size_t* p = reinterpret_cast<size_t*>( malloc( size + sizeof( size_t ) ) );

  if( p == null ) {
    throw std::bad_alloc();
  }

  ++oz::Alloc::count;
  oz::Alloc::amount += size;

  ++oz::Alloc::sumCount;
  oz::Alloc::sumAmount += size;

  oz::Alloc::maxCount = oz::max( oz::Alloc::count, oz::Alloc::maxCount );
  oz::Alloc::maxAmount = oz::max( oz::Alloc::amount, oz::Alloc::maxAmount );

  p[0] = size;
  return p + 1;
}

void operator delete ( void* ptr ) throw()
{
  size_t* chunk = reinterpret_cast<size_t*>( ptr ) - 1;

  --oz::Alloc::count;
  oz::Alloc::amount -= chunk[0];

  free( chunk );
}

void operator delete[] ( void* ptr ) throw()
{
  size_t* chunk = reinterpret_cast<size_t*>( ptr ) - 1;

  --oz::Alloc::count;
  oz::Alloc::amount -= chunk[0];

  free( chunk );
}

#endif
