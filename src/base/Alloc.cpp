/*
 *  Alloc.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "base.h"

#ifndef OZ_ALLOC

namespace oz
{

  int  Alloc::count   = -1;
  long Alloc::max     = -1;
  long Alloc::current = -1;

}

#else

#include <cstdlib>

namespace oz
{

  int  Alloc::count   = 0;
  long Alloc::max     = 0;
  long Alloc::current = 0;

}

void *operator new ( oz::uint size )
{
  oz::Alloc::count++;
  oz::Alloc::max     += size;
  oz::Alloc::current += size;
  return malloc( size );
}

void *operator new[] ( oz::uint size )
{
  oz::Alloc::count++;
  oz::Alloc::max     += size;
  oz::Alloc::current += size;
  return malloc( size );
}

void operator delete ( void *ptr )
{
  free( ptr );
}

void operator delete[] ( void *ptr )
{
  free( ptr );
}

#endif
