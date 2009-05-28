/*
 *  ObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *
 *  $Id$
 */

#pragma once

#include "Object.h"

namespace oz
{

  struct ObjectClass
  {
    static const int AUDIO_ARGS = 16;

    typedef ObjectClass *( *InitFunc )( const String &name, Config *config );

    String name;
    String description;

    Vec3   dim;
    int    flags;
    int    type;
    float  damage;

    String modelType;
    String modelPath;

    String audioType;
    String audioArgs[AUDIO_ARGS];

    virtual ~ObjectClass();

    static ObjectClass *init( const String &name, Config *config );
    virtual Object *create( const Vec3 &pos );
  };

}
