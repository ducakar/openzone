/*
 *  Structure.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix.h"

namespace oz
{

  class Structure : public Bounds
  {
    private:

      static const float DAMAGE_TRESHOLD = 400.0f;

    public:

      enum Rotation
      {
        R0   = 0,
        R90  = 1,
        R180 = 2,
        R270 = 3
      };

      int      index;
      int      bsp;
      Vec3     p;
      Rotation rot;
      float    life;

      explicit Structure() {}

      explicit Structure( int bsp_, const Vec3 &p_, Rotation rot_ ) :
          index( -1 ), bsp( bsp_ ), p( p_ ), rot( rot_ )
      {}

      explicit Structure( int bsp_, InputStream *istream ) : bsp( bsp_ )
      {
        readFull( istream );
      }

      void damage( float damage )
      {
        damage -= DAMAGE_TRESHOLD;

        if( damage > 0.0f ) {
          life -= damage;

          if( life <= 0.0f ) {
            destroy();
          }
        }
      }

      void destroy();

      void readFull( InputStream *istream );
      void writeFull( OutputStream *ostream );

  };

}
