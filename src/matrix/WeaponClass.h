/*
 *  WeaponClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "DynamicClass.h"

namespace oz
{

  struct WeaponClass : DynamicClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::WEAPON_BIT | Object::ITEM_BIT |
        Object::UPDATE_FUNC_BIT | Object::USE_FUNC_BIT;

    String onShot;

    int    nShots;
    float  shotInterval;

    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( int index, const Vec3 &pos );
    virtual Object *create( int index, InputStream *istream );
  };

}
