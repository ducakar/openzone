/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/Object.h"

namespace oz
{
namespace client
{

  class Audio
  {
    friend class List<Audio, 0>;

    protected:

      static const float REFERENCE_DISTANCE;

    public:

      typedef Audio *( *CreateFunc )( const Object *object );

    private:

      Audio *next[1];

    protected:

      void playSound( int sample, float volume, const Object *obj ) const;
      void playContSound( int sample, float volume, uint key, const Object *obj ) const;
      void requestSounds() const;
      void releaseSounds() const;

      explicit Audio( const Object *obj );

    public:

      const Object *obj;
      bint  isUpdated;

      virtual ~Audio();

      virtual void play( const Audio *parent ) = 0;

  };

}
}
