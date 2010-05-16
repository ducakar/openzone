/*
 *  WeaponClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/DynamicClass.h"

namespace oz
{

  class WeaponClass : public DynamicClass
  {
    private:

      static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::WEAPON_BIT | Object::ITEM_BIT |
          Object::UPDATE_FUNC_BIT | Object::USE_FUNC_BIT;

    public:

      String onShot;

      int    nShots;
      float  shotInterval;

      static ObjectClass* init( const String& name, const Config* config );

      virtual Object* create( int index, const Vec3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;
  };

}
