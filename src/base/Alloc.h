/*
 *  Alloc.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

namespace oz
{

  class Alloc
  {
    private:

      Alloc() {};
      Alloc( const Alloc& ) {};

    public:

      static int  count;
      static long max;
      static long current;

  };

}

void *operator new ( oz::uint size );
void *operator new[] ( oz::uint size );
void operator delete ( void *ptr );
void operator delete[] ( void *ptr );

// placement operators
inline void *operator new ( oz::uint, void *pos ) { return pos; }
inline void *operator new[] ( oz::uint, void *pos ) { return pos; }
inline void operator delete ( void*, void* ) {}
inline void operator delete[] ( void*, void* ) {}
