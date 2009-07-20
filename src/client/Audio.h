/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "matrix/Object.h"

namespace oz
{
namespace client
{

  // TODO buffer clearing
  class Audio
  {
    public:

      typedef Audio *( *InitFunc )( const Object *object );

    protected:

      const Object      *obj;
      const ObjectClass *clazz;

      void playSound( int sample, float volume ) const;
      void playContSound( int sample, float volume, uint key ) const;
      void requestSounds() const;
      void releaseSounds() const;

    public:

      Audio( const Object *obj, const ObjectClass *clazz );
      virtual ~Audio();

      virtual void update() = 0;

  };

}
}
