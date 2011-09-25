/*
 *  ObjectClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"

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

      static const int INVENTORY_ITEMS = 100;

    public:

      // 00 <= AUDIOARGS <= 99 (two decimal digits)
      static const int AUDIO_SAMPLES = 32;

      typedef ObjectClass* ( * InitFunc )( const Config* config );

      String name;
      String title;
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

      int    nDebris;
      float  debrisVelocitySpread;
      float  debrisRejection;
      float  debrisMass;
      float  debrisLifeTime;
      float  debrisColourSpread;
      Vec3   debrisColour;

      String modelType;
      int    modelIndex;

      String audioType;
      int    audioSamples[AUDIO_SAMPLES];

      int    nItems;
      Vector<String> items;

    protected:

      void fillCommonConfig( const Config* config );
      void fillCommonFields( Object* obj ) const;

    public:

      virtual ~ObjectClass();

      static ObjectClass* init( const Config* config );

      virtual Object* create( int index, const Point3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
