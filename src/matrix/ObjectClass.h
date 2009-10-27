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

#include "io.h"

namespace oz
{

  class Object;

  struct ObjectClass
  {
    // 00 <= AUDIOARGS <= 99 (two decimal digits)
    static const int AUDIO_SAMPLES = 32;

    static const int BASE_FLAGS = 0;
    static const int DEFAULT_FLAGS;

    typedef ObjectClass *( *InitFunc )( const String &name, const Config *config );

    String name;
    String description;

    Vec3   dim;
    int    flags;
    int    type;
    float  life;
    float  damageTreshold;
    float  damageRatio;

    int    nDebris;
    float  debrisVelocitySpread;
    float  debrisRejection;
    float  debrisMass;
    float  debrisLifeTime;
    Vec3   debrisColor;
    float  debrisColorSpread;

    String modelType;
    String modelName;

    String audioType;
    int    audioSamples[AUDIO_SAMPLES];

    virtual ~ObjectClass();

    static void fill( ObjectClass *clazz, const Config *config );
    static ObjectClass *init( const String &name, const Config *config );

    virtual Object *create( const Vec3 &pos );
    virtual Object *create( InputStream *istream );
  };

}
