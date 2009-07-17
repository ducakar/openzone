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

  class Audio
  {
    public:

      typedef Audio *( *InitFunc )( const Object *object );

    protected:

      const Object *object;

      void playSoundEvent( int sample ) const;
      void playSoundEffect( int sample, uint key ) const;
      void requestSounds() const;
      void releaseSounds() const;

    public:

      virtual ~Audio();

      virtual void update() = 0;

  };

}
}
