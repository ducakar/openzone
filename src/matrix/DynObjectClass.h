/*
 *  DynObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "ObjectClass.h"

namespace oz
{

  struct DynObjectClass : ObjectClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT;
    static const int DEFAULT_FLAGS = Object::CLIP_BIT;

    float mass;
    float lift;

    static ObjectClass *init( const String &name, Config *config );
    virtual Object *create( const Vec3 &pos );
    virtual Object *create( InputStream *istream );
  };

}
