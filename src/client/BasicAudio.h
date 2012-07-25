/*
 *  BasicAudio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Audio.h"

namespace oz
{
namespace client
{

  class BasicAudio : public Audio
  {
    protected:

      static const int SND_FRICTING = Object::EVENT_FRICTING;

      BasicAudio( const Object *obj ) : Audio( obj ) {}

    public:

      static Audio *create( const Object *obj );

      void play( const Audio *parent );

  };

}
}
