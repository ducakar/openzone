/*
 *  Alloc.cpp
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "base.h"

#include <cstdlib>

namespace oz
{

  int  Alloc::count     = 0;
  long Alloc::amount    = 0;

  int  Alloc::sumCount  = 0;
  long Alloc::sumAmount = 0;

  int  Alloc::maxCount  = 0;
  long Alloc::maxAmount = 0;

  void* ( *const Alloc::malloc )( uint )         = ::malloc;
  void  ( *const Alloc::free )( void* )          = ::free;
  void* ( *const Alloc::realloc )( void*, uint ) = ::realloc;

}

#ifndef OZ_ALLOC_STATISTICS

using oz::uint;

void* operator new ( uint size )
{
  return malloc( size );
}

void* operator new[] ( uint size )
{
  return malloc( size );
}

void operator delete[] ( void* ptr )
{
  free( ptr );
}

void operator delete ( void* ptr )
{
  free( ptr );
}

#else

using oz::uint;
using oz::max;
using oz::Alloc;

void* operator new ( uint size )
{
  uint* p = reinterpret_cast<uint*>( malloc( size + sizeof( uint ) ) );

  if( p == null ) {
    throw Exception( "Bad allocation" );
  }

  Alloc::count++;
  Alloc::amount += size;

  Alloc::sumCount++;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  p[0] = size;
  return p + 1;
}

void* operator new[] ( uint size )
{
  uint* p = reinterpret_cast<uint*>( malloc( size + sizeof( uint ) ) );

  if( p == null ) {
    throw Exception( "Bad allocation" );
  }

  Alloc::count++;
  Alloc::amount += size;

  Alloc::sumCount++;
  Alloc::sumAmount += size;

  Alloc::maxCount = max( Alloc::count, Alloc::maxCount );
  Alloc::maxAmount = max( Alloc::amount, Alloc::maxAmount );

  p[0] = size;
  return p + 1;
}

void operator delete ( void* ptr )
{
  uint* chunk = reinterpret_cast<uint*>( ptr ) - 1;

  Alloc::count--;
  Alloc::amount -= chunk[0];

  free( chunk );
}

void operator delete[] ( void* ptr )
{
  uint* chunk = reinterpret_cast<uint*>( ptr ) - 1;

  Alloc::count--;
  Alloc::amount -= chunk[0];

  free( chunk );
}

#endif
