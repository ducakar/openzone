/*
 *  Audio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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

      static const int   UPDATED_BIT = 0x00000001;

    protected:

      static const float REFERENCE_DISTANCE = 4.0f;

    public:

      typedef Audio* ( *CreateFunc )( const Object* object );

      Audio* next[1];

    protected:

      void playSound( int sample, float volume, const Object* obj ) const;
      void playContSound( int sample, float volume, uint key, const Object* obj ) const;
      void requestSounds() const;
      void releaseSounds() const;

      explicit Audio( const Object* obj );

    public:

      const Object* obj;
      int flags;

      virtual ~Audio();

      virtual void play( const Audio* parent ) = 0;

  };

}
}
