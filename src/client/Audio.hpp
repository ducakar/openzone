/*
 *  Audio.hpp
 *
 *  Audio analogue to Model class
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"

namespace oz
{
namespace client
{

  class Audio
  {
    public:

      static const int   UPDATED_BIT = 0x00000001;

      static const float REFERENCE_DISTANCE;
      static const float ROLLOFF_FACTOR;

      typedef Audio* ( * CreateFunc )( const Object* object );

    protected:

      // obj: source object of the effect, parent: object at which the effect is played
      // obj != parent: e.g. an object obj in the inventory of bot parent plays a sound
      void playSound( int sample, float volume, const Object* obj, const Object* parent ) const;
      void playContSound( int sample, float volume, const Object* obj, const Object* parent ) const;
      void playEngineSound( int sample, float volume, float pitch, const Object* obj ) const;

      explicit Audio( const Object* obj );

    public:

      const Object*      obj;
      const ObjectClass* clazz;
      int                flags;

      virtual ~Audio();

      virtual void play( const Audio* parent ) = 0;

  };

}
}
