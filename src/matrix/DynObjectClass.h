/*
 *  DynObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Object.h"
#include "ObjectClass.h"

namespace oz
{

  struct DynObjectClass : ObjectClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT;
    static const int DEFAULT_FLAGS = Object::CLIP_BIT;

    float mass;
    float lift;

    static void fill( DynObjectClass *clazz, const Config *config );
    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( const Vec3 &pos );
    virtual Object *create( InputStream *istream );
  };

}
