/*
 *  HealthItemClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "DynObjectClass.h"

namespace oz
{

  struct HealthItemClass : DynObjectClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::ITEM_BIT;
    static const int DEFAULT_FLAGS = Object::CLIP_BIT;

    float health;
    float stamina;

    static void fill( HealthItemClass *clazz, const Config *config );
    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( const Vec3 &pos );
    virtual Object *create( InputStream *istream );
  };

}

