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

  struct BasicAudio : Audio
  {
    /*
     * STANDARD SOUND SLOTS
     */
    static const int SND_DESTROY  = 0;
    static const int SND_HIT      = 1;
    static const int SND_SPLASH   = 2;
    static const int SND_FRICTING = 3;

    static const float MOMENTUM_INTENSITY_FACTOR;

    BasicAudio( const Object *obj ) : Audio( obj ) {}

    static Audio *create( const Object *obj );

    void update();
  };

}
}
