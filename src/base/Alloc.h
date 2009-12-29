/*
 *  Alloc.h
 *
 *  Overload default new and delete operators for slightly better performance (ifndef OZ_ALLOC) or
 *  provide heap allocation statistics (ifdef OZ_ALLOC).
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

namespace oz
{

  struct Alloc
  {
    private:

      // static class
      Alloc() {}
      Alloc( const Alloc& ) {};

    public:

      static int  count;
      static long amount;

      static int  sumCount;
      static long sumAmount;

      static int  maxCount;
      static long maxAmount;

      static void* ( *const malloc )( uint size );
      static void  ( *const free )( void* ptr );
      static void* ( *const realloc )( void* ptr, uint size );

  };

}
