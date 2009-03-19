/*
 *  LadderClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#include "precompiled.hpp"

#include "LadderClass.hpp"

namespace oz
{

  Class *LadderClass::init( Config *config_ )
  {
    Config &config = *config_;
    LadderClass *clazz = new LadderClass();

    OZ_CLASS_READ_FLOAT( clazz, dim.x, 0.5f );
    OZ_CLASS_READ_FLOAT( clazz, dim.y, 0.5f );
    OZ_CLASS_READ_FLOAT( clazz, dim.z, 0.5f );

    OZ_CLASS_READ_INT( clazz, flags, Object::CLIP_BIT );
    OZ_CLASS_READ_INT( clazz, type, 0 );
    OZ_CLASS_READ_FLOAT( clazz, damage, 1.0f );

    OZ_CLASS_READ_STRING( clazz, model, "mdl/goblin.md2" );

    return clazz;
  }

  Object *LadderClass::create( const Vec3 &pos )
  {
    Object *obj = new Object();

    obj->p = pos;
    obj->dim = dim;

    obj->flags = flags;
    obj->type = type;
    obj->damage = damage;

    return obj;
  }

}
