/*
 *  Alloc.cpp
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "oz.h"

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

void* operator new ( oz::size_t size )
{
  return malloc( size );
}

void* operator new[] ( oz::size_t size )
{
  return malloc( size );
}

void operator delete ( void* ptr )
{
  free( ptr );
}

void operator delete[] ( void* ptr )
{
  free( ptr );
}

#else

void* operator new ( oz::size_t size )
{
  oz::size_t* p = reinterpret_cast<oz::size_t*>( malloc( size + sizeof( oz::size_t ) ) );

  if( p == null ) {
    throw Exception( "Bad allocation" );
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

void* operator new[] ( oz::size_t size )
{
  oz::size_t* p = reinterpret_cast<oz::size_t*>( malloc( size + sizeof( oz::size_t ) ) );

  if( p == null ) {
    throw Exception( "Bad allocation" );
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

void operator delete ( void* ptr )
{
  oz::size_t* chunk = reinterpret_cast<oz::size_t*>( ptr ) - 1;

  --oz::Alloc::count;
  oz::Alloc::amount -= chunk[0];

  free( chunk );
}

void operator delete[] ( void* ptr )
{
  oz::size_t* chunk = reinterpret_cast<oz::size_t*>( ptr ) - 1;

  --oz::Alloc::count;
  oz::Alloc::amount -= chunk[0];

  free( chunk );
}

#endif
