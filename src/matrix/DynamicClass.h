/*
 *  DynamicClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Object.h"
#include "ObjectClass.h"

namespace oz
{

  struct DynamicClass : ObjectClass
  {
    static const int BASE_FLAGS = Object::DYNAMIC_BIT;

    float mass;
    float lift;

    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( int index, const Vec3 &pos );
    virtual Object *create( int index, InputStream *istream );
  };

}
