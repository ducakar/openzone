/*
 *  ObjectClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "matrix/common.h"

#define OZ_CLASS_SET_FLAG( flagBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->flags |= flagBit; \
  }

namespace oz
{

  class Object;

  class ObjectClass
  {
    private:

      static const int BASE_FLAGS = 0;

    public:

      // 00 <= AUDIOARGS <= 99 (two decimal digits)
      static const int AUDIO_SAMPLES = 32;

      typedef ObjectClass* ( *InitFunc )( const String& name, const Config* config );

      String name;
      String description;

      Vec3   dim;
      int    flags;
      int    type;
      float  life;
      float  damageThreshold;

      String onDestroy;
      String onDamage;
      String onHit;
      String onUse;
      String onUpdate;
      String onAct;

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

      static void fillCommon( ObjectClass* clazz, const Config* config );
      static ObjectClass* init( const String& name, const Config* config );

      virtual Object* create( int index, const Vec3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
