/*
 *  SimpleAudio.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "Audio.h"

namespace oz
{
namespace client
{

  struct SimpleAudio : Audio
  {
    /*
     * STANDARD SOUND SLOTS
     */
    static const int SND_HIT_SOFT       = 0;
    static const int SND_HIT_HARD       = 1;
    static const int SND_SPLASH_SOFT    = 2;
    static const int SND_SPLASH_HARD    = 3;
    static const int SND_FRICTING       = 4;

    SimpleAudio( const Object *obj, const ObjectClass *clazz ) : Audio( obj, clazz ) {}

    static Audio *create( const Object *obj );

    void update();
  };

}
}
