/*
 *  Weapon.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Dynamic.hpp"
#include "matrix/WeaponClass.hpp"

namespace oz
{

  class Weapon : public Dynamic
  {
    protected:

      virtual void onUpdate();
      virtual void onUse( Bot* user );

    public:

      static const int EVENT_SHOT       = 7;
      static const int EVENT_SHOT_EMPTY = 8;

      static Pool<Weapon, 1024> pool;

      // -1: unlimited
      int   nShots;
      float shotTime;

      Weapon();

      void trigger( Bot* user );

      virtual void readFull( InputStream* istream );
      virtual void writeFull( OutputStream* ostream ) const;
      virtual void readUpdate( InputStream* istream );
      virtual void writeUpdate( OutputStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

  };

}
