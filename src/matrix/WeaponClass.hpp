/*
 *  WeaponClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/DynamicClass.hpp"

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

      static ObjectClass* init( const Config* config );

      virtual Object* create( int index, const Point3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;
  };

}
