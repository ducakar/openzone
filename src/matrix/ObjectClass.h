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
    // 00 <= AUDIOARGS <= 99 (two decimal digits)
    static const int AUDIO_SAMPLES = 16;

    static const int BASE_FLAGS = 0;
    static const int DEFAULT_FLAGS = Object::CLIP_BIT;

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
    int    audioSamples[AUDIO_SAMPLES];

    virtual ~ObjectClass();

    static ObjectClass *init( const String &name, Config *config );
    virtual Object *create( const Vec3 &pos );
    virtual Object *create( InputStream *istream );
  };

}
