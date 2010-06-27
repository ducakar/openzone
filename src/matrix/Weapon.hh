/*
 *  Weapon.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/Dynamic.hh"
#include "matrix/WeaponClass.hh"

namespace oz
{

  class Weapon : public Dynamic
  {
    protected:

      virtual void onUpdate();
      virtual void onUse( Bot* user );
      virtual void onShot( Bot* user );

    public:

      static const int EVENT_SHOT       = 7;
      static const int EVENT_SHOT_EMPTY = 8;

      static Pool<Weapon> pool;

      // -1: unlimited
      int   nShots;
      float shotTime;

      explicit Weapon() {}

      void trigger( Bot* user )
      {
        assert( user != null );

        if( shotTime == 0.0f ) {
          const WeaponClass* clazz = static_cast<const WeaponClass*>( type );

          shotTime = clazz->shotInterval;

          if( nShots == 0 ) {
            addEvent( EVENT_SHOT_EMPTY, 1.0f );
          }
          else {
            nShots = max( -1, nShots - 1 );

            addEvent( EVENT_SHOT, 1.0f );
            onShot( user );
          }
        }
      }

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
